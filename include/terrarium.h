//
// Created by @redrossa on 2021-06-20.
//

#ifndef MAPTILE_TERRARIUM_H
#define MAPTILE_TERRARIUM_H

#include "download.h"

#include <stdarg.h>
#include <curl/curl.h>

namespace terrarium
{
    using namespace maptile;

    char *tile_url_init(map *m, index_t x, index_t y);

    byte_t *png_decode(size_t *nmemb, size_t *pixw_dst, size_t *pixh_dst, byte_t *img, size_t size);

    int direct_tile_fflush(index_t itc, map *m, tile_transfer_t *tile_transf, va_list args);

    int map_fflush(index_t itc, map *m, tile_transfer_t *tile_transf, va_list args);
}

#endif //MAPTILE_TERRARIUM_H
