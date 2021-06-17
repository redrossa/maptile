//
// Created by @redrossa on 6/10/21.
//

#include "../include/download.h"

#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

static size_t write_cb(void * data, size_t size, size_t nmemb, void * dest)
{
    size_t realsize = size * nmemb;
    tile_data_t * td = (tile_data_t *) dest;

    unsigned char * ptr = realloc(td->data, td->size + realsize + 1);
    if(ptr == NULL)
        return 0;  /* out of memory! */

    td->data = ptr;
    memcpy(&(td->data[td->size]), data, realsize);
    td->size += realsize;
    td->data[td->size] = 0;

    return realsize;
}

static int setup_easy(CURL * eh, tile_t * tile)
{
    /* Initialize url */
    char url[100];
    tile_url(tile, url);

    /* Initialize object to attach to easy handler */
    tile_data_t * td = malloc(sizeof(tile_data_t));
    if (!td)
        return 0;
    td->info = tile;
    td->size = 0;
    td->data = NULL;

    /* Set easy handler options */
    curl_easy_setopt(eh, CURLOPT_URL, url);
    curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(eh, CURLOPT_WRITEDATA, td);
    curl_easy_setopt(eh, CURLOPT_PRIVATE, td);

    return 1;
}

int download(map_t * map, size_t (*flush)(tile_data_t *, va_list), ...)
{
    CURLcode code;
    CURL * eh = curl_easy_init();
    if (!eh)
        return -1;

    tile_t tile;
    int successes = 0;
    int c;

    va_list args;
    va_start(args, flush);

#pragma omp parallel for
    for (int i = 0; i < map->tile_count; i++)
    {
        tile_fromindex(&tile, map, i);
        c = setup_easy(eh, &tile);
        if (!c)
            break;
        code = curl_easy_perform(eh);

        tile_data_t * td;
        curl_easy_getinfo(eh, CURLINFO_PRIVATE, &td);
        if (code == CURLE_OK && (!flush || (flush && (*flush)(td, args) == td->size)))
            successes++;
        else
            i--; /* Retry this tile */

        free(td->data);
        free(td);
    }

    curl_easy_cleanup(eh);
    va_end(args);

    return successes;
}

static CURL * multi_add_transfer(CURLM * cm, map_t * map, int i)
{
    /* Initialize tile */
    tile_t * tile = malloc(sizeof(tile_t));
    if (!tile)
        return NULL;
    tile_fromindex(tile, map, i);

    /* Initialize easy handler */
    CURL * eh = curl_easy_init();
    if (!eh)
    {
        free(tile);
        return NULL;
    }

    /* Set easy handler options */
    setup_easy(eh, tile);
    /* HTTP/2 please */
    curl_easy_setopt(eh, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
#if (CURLPIPE_MULTIPLEX > 0)
    /* wait for pipe connection to confirm */
    curl_easy_setopt(eh, CURLOPT_PIPEWAIT, 1L);
#endif
    curl_multi_add_handle(cm, eh);

    return eh;
}

int download_multi(map_t * map, size_t (*flush)(tile_data_t *, va_list), ...)
{
    CURLM * cm;
    CURLMsg * msg;
    int msgs_left = 0;
    int still_alive = 0;
    int transfers;
    const int nparallel = map->tile_count < DOWNLOAD_MAX_PARALLEL ? map->tile_count : DOWNLOAD_MAX_PARALLEL;

    cm = curl_multi_init();
    if (!cm)
        return -1;

    /* Limit the amount of simultaneous connections curl should allow: */
    curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, nparallel);

    curl_multi_setopt(cm, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);

    CURL ** ehs = malloc(sizeof(CURL *) * nparallel);
    if (!ehs)
        return -1;

    /* Will only use nparallel number of easy transfers since curl_easy_init() is costly */
    for (transfers = 0; transfers < nparallel; transfers++)
    {
        CURL * eh = multi_add_transfer(cm, map, transfers);
        if (!eh)
        {
            for (int i = 0; i < transfers; i++)
            {
                curl_multi_remove_handle(cm, ehs[i]);
                tile_data_t * td;
                curl_easy_getinfo(ehs[i], CURLINFO_PRIVATE, &td);
                free(td->info);
                free(td);
                curl_easy_cleanup(ehs[i]);
            }
            free(ehs);
            curl_multi_cleanup(cm);
            return -1;
        }
        ehs[transfers] = eh;
    }

    int successes = 0;
    va_list args;
    va_start(args, flush);

    do {
        curl_multi_perform(cm, &still_alive);

        while ((msg = curl_multi_info_read(cm, &msgs_left)))
        {
            if (msg->msg == CURLMSG_DONE)
            {
                CURLcode code = msg->data.result;
                CURL * eh = msg->easy_handle;
                curl_multi_remove_handle(cm, eh);

                tile_data_t * td;
                curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &td);

                if (code == CURLE_OK && (!flush || (flush && (*flush)(td, args) == td->size)))
                {
                    successes++;

                    free(td->info);
                    free(td->data);
                    free(td);

                    /* Reuse easy handler for the next tile */
                    if (transfers < map->tile_count)
                    {
                        tile_t * tile = malloc(sizeof(tile_t));
                        if (!tile)
                        {
                            transfers++; /* Skip next tile, will be reflected on successes */
                            continue;
                        }
                        tile_fromindex(tile, map, transfers++);
                        setup_easy(eh, tile);
                        curl_multi_add_handle(cm, eh);
                    }
                    else /* Already connected for all tiles */
                        curl_easy_cleanup(eh);
                }
                else
                {
                    free(td->data);
                    td->data = NULL;
                    td->size = 0;
                    curl_multi_add_handle(cm, eh); /* Try the same url again */
                }
            }
        }
        if (still_alive)
            curl_multi_wait(cm, NULL, 0, 100, NULL);

    } while (still_alive);

    va_end(args);
    curl_multi_cleanup(cm);

    return successes;
}
