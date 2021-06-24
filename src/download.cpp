//
// Created by @redrossa on 2021-06-20.
//

#include "../include/download.h"

#include "../include/indicators.hpp"

#include <vector>
#include <cstdarg>

#define DOWNLOAD_MAX_PARALLEL 128

using namespace maptile;

transfer::transfer(index_t i)
{
    this->i = i;
    data = nullptr;
    size = 0;
}

transfer::~transfer()
{
    free(data);
    data = nullptr;
    size = 0;
}

void transfer::append_data(void* chunk, size_t chunk_size)
{
    auto* ptr = static_cast<byte_t*>(realloc(data, size + chunk_size));
    if (!ptr)
        throw std::bad_alloc();
    data = ptr;
    memcpy(&(data[size]), chunk, chunk_size);
    size += chunk_size;
}

void transfer::get_data(byte_t* dst)
{
    memcpy(dst, data, size);
}

int maptile::verbose_flush(index_t itc, map* m, tile_transfer_t* tile_transf, va_list args)
{
    using namespace indicators;

    static ProgressBar bar{
        option::BarWidth{50},
        option::Start{"\r["},
        option::Fill{"="},
        option::Lead{">"},
        option::Remainder{"."},
        option::End{"]"},
        option::ShowPercentage{true},
        option::MaxPostfixTextLen{256},
        option::ShowElapsedTime{true},
        option::ShowRemainingTime{true},
    };

    static tile_handle_fnp handlefn = va_arg(args, tile_handle_fnp);

    if (itc == 1)
        show_console_cursor(false);

    int ret = (*handlefn)(itc, m, tile_transf, args);

    if (ret)
    {
        bar.set_option(option::PostfixText{"Downloaded " + std::to_string(itc) + "/" + std::to_string(m->get_tile_count()) + " tiles"});
        bar.set_progress(((double) itc/m->get_tile_count() * 100));
    }

    if (itc == m->get_tile_count())
        show_console_cursor(true);

    return ret;
}

static size_t write_cb(void* data, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    auto* mem = (tile_transfer_t*) userp;

    auto* ptr = static_cast<byte_t*>(realloc(mem->data, mem->size + realsize + 1));
    if (!ptr)
        return 0;  /* out of memory! */

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), data, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

static int setup_transfer(CURL* eh, const std::function<transfer*(maptile::index_t)>& transf_builder, index_t id)
{
    if (!eh)
        return 0;

    index_t x, y;
    m->tile_coords_from_index(&x, &y, transfers);

    char* url = (*url_initfnp)(m, x, y);
    if (!url)
        return 0;

    auto* priv = static_cast<tile_transfer_t*>(malloc(sizeof(tile_transfer_t)));
    if (!priv) {
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

static void cleanup_tranfer(CURL* eh)
{

}

template<typename T>
int maptile::download(const transfer_builder& build,
                      const transfer_handler& handler,
                      size_t nparallel)
{
    CURLM* cm;
    CURLMsg* msg;
    int msgs_left = 0;
    int still_alive = 0;
    index_t transfers;
    nparallel = nparallel > ;

    cm = curl_multi_init();
    if (!cm)
        throw std::bad_alloc();

    /* Limit the amount of simultaneous connections curl should allow: */
    curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, nparallel);

    curl_multi_setopt(cm, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);

    /* Initialize easy handler container */
    std::vector<CURL*> ehs;
    ehs.reserve(nparallel);

    /* Will only use nparallel number of easy transfers since curl_easy_init() is costly */
    for (transfers = 0; transfers < nparallel; transfers++)
    {
        CURL* eh = curl_easy_init();
        int ok = setup_transfer(eh, build, transfers);
        if (!ok)
        {
            for (auto& eh : ehs)
                cleanup_tranfer(eh);
            if (eh)
                curl_easy_cleanup(eh);
            curl_multi_cleanup(cm);
            throw std::bad_alloc();
        }
        ehs.push_back(eh);
        curl_multi_add_handle(cm, eh);
    }

    int successes = 0;
    index_t itc = 0;

    do {
        curl_multi_perform(cm, &still_alive);

        while ((msg = curl_multi_info_read(cm, &msgs_left))) {
            if (msg->msg != CURLMSG_DONE)
                continue;

            CURLcode code = msg->data.result;
            CURL* eh = msg->easy_handle;
            curl_multi_remove_handle(cm, eh);
            maptile::transfer* priv;
            curl_easy_getinfo(eh, CURLINFO_PRIVATE, &priv);
            itc++;

            if (code == CURLE_OK) {
                if (handler(itc, priv))
                    successes++;

                /* Reuse easy handler for the next tile */
                if (transfers < m->get_tile_count()) {
                    index_t x, y;
                    m->tile_coords_from_index(&x, &y, transfers++);

                    char* url = (*url_initfn)(m, x, y);
                    if (!url) {
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
}

int maptile::download_tiles(map* m, tile_url_init_fnp url_initfn, tile_handle_fnp handlefn, ...) {









    va_end(args);
    curl_multi_cleanup(cm);

    return successes;
}