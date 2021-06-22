//
// Created by @redrossa on 2021-06-20.
//

#include "../include/download.h"

#include <vector>
#include <cstdarg>

#define DOWNLOAD_MAX_PARALLEL 128

using namespace maptile;

static size_t write_cb(void *data, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    auto *mem = (tile_transfer_t *) userp;

    auto *ptr = static_cast<byte_t *>(realloc(mem->data, mem->size + realsize + 1));
    if (!ptr)
        return 0;  /* out of memory! */

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), data, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

static int setup_transfer(CURL *eh, map *m, tile_url_init_fnp url_initfnp, index_t transfers)
{
    if (!eh)
        return 0;

    index_t x, y;
    m->tile_coord_from_index(&x, &y, transfers);

    char *url = (*url_initfnp)(m, x, y);
    if (!url)
        return 0;

    auto *priv = static_cast<tile_transfer_t *>(malloc(sizeof(tile_transfer_t)));
    if (!priv)
    {
        free(url);
        return 0;
    }
    priv->z = m->get_zoom();
    priv->x = x;
    priv->y = y;
    priv->data = nullptr;
    priv->size = 0;

    curl_easy_setopt(eh, CURLOPT_URL, url);
    curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(eh, CURLOPT_WRITEDATA, priv);
    curl_easy_setopt(eh, CURLOPT_PRIVATE, priv);
    curl_easy_setopt(eh, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
#if (CURLPIPE_MULTIPLEX > 0)
    curl_easy_setopt(eh, CURLOPT_PIPEWAIT, 1L); /* wait for pipe connection to confirm */
#endif

    free(url);
    return 1;
}

int maptile::download_tiles(map *m, tile_url_init_fnp url_initfnp, tile_handle_fnp handlefnp, ...)
{
    CURLM *cm;
    CURLMsg *msg;
    int msgs_left = 0;
    int still_alive = 0;
    index_t transfers;
    const size_t nparallel = m->get_tile_count() < DOWNLOAD_MAX_PARALLEL ? m->get_tile_count() : DOWNLOAD_MAX_PARALLEL;

    cm = curl_multi_init();
    if (!cm)
        throw std::bad_alloc();

    /* Limit the amount of simultaneous connections curl should allow: */
    curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, nparallel);

    curl_multi_setopt(cm, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);

    /* Initialize easy handler container */
    std::vector<CURL *> ehs;
    ehs.reserve(nparallel);

    /* Will only use nparallel number of easy transfers since curl_easy_init() is costly */
    for (transfers = 0; transfers < nparallel; transfers++)
    {
        CURL *eh = curl_easy_init();
        int ok = setup_transfer(eh, m, url_initfnp, transfers);
        if (!eh || !ok)
        {
            for (auto& eh : ehs)
                curl_easy_cleanup(eh);
            curl_multi_cleanup(cm);
            throw std::bad_alloc();
        }
        ehs.push_back(eh);
        curl_multi_add_handle(cm, eh);
    }

    int successes = 0;
    index_t itc = 0;
    va_list args;
            va_start(args, handlefnp);

    do
    {
        curl_multi_perform(cm, &still_alive);

        while ((msg = curl_multi_info_read(cm, &msgs_left)))
        {
            if (msg->msg != CURLMSG_DONE)
                continue;

            CURLcode code = msg->data.result;
            CURL *eh = msg->easy_handle;
            curl_multi_remove_handle(cm, eh);
            tile_transfer_t *priv;
            curl_easy_getinfo(eh, CURLINFO_PRIVATE, &priv);
            itc++;

            if (code == CURLE_OK)
            {
                if ((*handlefnp)(itc, m, priv, args))
                    successes++;

                /* Reuse easy handler for the next tile */
                if (transfers < m->get_tile_count())
                {
                    index_t x, y;
                    m->tile_coord_from_index(&x, &y, transfers++);

                    char *url = (*url_initfnp)(m, x, y);
                    if (!url)
                    {
                        free(priv->data);
                        free(priv);
                        curl_easy_cleanup(eh);
                        transfers = m->get_tile_count(); /* Abort next tile transfers */
                        continue;
                    }

                    priv->x = x;
                    priv->y = y;
                    free(priv->data);
                    priv->data = nullptr;
                    priv->size = 0;

                    curl_easy_setopt(eh, CURLOPT_URL, url);
                    curl_multi_add_handle(cm, eh); /* Re-add this easy handler to the multi handler */
                    free(url);
                } else /* Already connected for all tiles */
                {
                    free(priv->data);
                    free(priv);
                    curl_easy_cleanup(eh);
                }
            } else /* Try the same url again */
            {
                free(priv->data);
                priv->data = nullptr;
                priv->size = 0;
                curl_multi_add_handle(cm, eh);
            }
        }
        if (still_alive)
            curl_multi_wait(cm, NULL, 0, 100, NULL);

    } while (still_alive);

            va_end(args);
    curl_multi_cleanup(cm);

    return successes;
}