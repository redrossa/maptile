//
// Created by @redrossa on 6/10/21.
//

#include "../include/download.h"

#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

struct memory
{
    unsigned char * response;
    size_t size;
};

static size_t write_cb(void * data, size_t size, size_t nmemb, void * dest)
{
    size_t realsize = size * nmemb;
    struct memory *mem = (struct memory *) dest;

    unsigned char * ptr = realloc(mem->response, mem->size + realsize + 1);
    if(ptr == NULL)
        return 0;  /* out of memory! */

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

int download(map_t * map, size_t (*flush)(unsigned char *, size_t, tile_t *, int i, va_list), ...)
{
    CURL * curl;
    CURLcode code;

    curl = curl_easy_init();
    if (!curl)
        return -1;

    int successes = 0;
    char url_buf[100];
    tile_t tile;
    struct memory resp_buf = {0};

    va_list args;
    va_start(args, flush);

#pragma omp parallel for
    for (int i = 0; i < map->tile_count; i++) {
        tile_fromindex(&tile, map, i);
        tile_url(&tile, url_buf);

        curl_easy_setopt(curl, CURLOPT_URL, url_buf);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_buf);

        code = curl_easy_perform(curl);
        if (code == CURLE_OK) {
            size_t res = flush ? (*flush)(resp_buf.response, resp_buf.size, &tile, i, args) : resp_buf.size;
            if (res == resp_buf.size)
                successes++;
        }

        if (resp_buf.size > 0)
        {
            free(resp_buf.response);
            resp_buf = (struct memory) {0};
        }
    }

    va_end(args);
    curl_easy_cleanup(curl);

    return successes;
}
