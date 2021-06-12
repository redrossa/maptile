//
// Created by @redrossa on 6/10/21.
//

#include "download.h"

#include "tile.h"

#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

int tile_filename(char * dest, char * dirname, tile_t * tile)
{
    int length = 0;
    length += sprintf(dest, "%s", dirname);
    length += sprintf(dest + length, "/");
    length += sprintf(dest + length, "%d", tile->zoom);
    length += sprintf(dest + length, "-");
    length += sprintf(dest + length, "%d", tile->x);
    length += sprintf(dest + length, "-");
    length += sprintf(dest + length, "%d", tile->y);
    length += sprintf(dest + length, ".png");
    return length;
}

size_t write_callback(char * data, size_t size, size_t nmemb, void * userp)
{
    size_t realsize = size * nmemb;
    return realsize;
}

int download(char * dirname, map_t * map)
{
    CURL * curl;
    CURLcode code;

    curl = curl_easy_init();

    if (!curl)
        return -1;

    int successes = 0;
    char url_buf[100];
    tile_t tile;

#pragma omp parallel for
    for (int i = 0; i < map->tile_count; i++)
    {
        tile_fromindex(&tile, map, i);

        char name[strlen(dirname) + 25];
        tile_filename(name, dirname, &tile);

        FILE * fp = fopen(name, "wb");
        if (!fp)
        {
            curl_easy_cleanup(curl);
            return successes;
        }

        tile_url(&tile, url_buf);
        curl_easy_setopt(curl, CURLOPT_URL, url_buf);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        code = curl_easy_perform(curl);
        if (code == CURLE_OK)
            successes++;

        fclose(fp);
    }

    curl_easy_cleanup(curl);

    return successes;
}
