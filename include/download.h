//
// Created by @redrossa on 2021-06-20.
//

#include "map.h"

#include <curl/curl.h>

#ifndef MAPTILE_DOWNLOAD_H
#define MAPTILE_DOWNLOAD_H

namespace maptile
{
    typedef struct
    {
        index_t z;
        index_t x;
        index_t y;
        byte_t *data;
        size_t size;
    } tile_transfer_t;

    typedef char *(*tile_url_init_fnp)(map *m, index_t x, index_t y);

    typedef int (*tile_handle_fnp)(index_t itc, map *m, tile_transfer_t *tile_transf, va_list args);

    int download_tiles(map *m, tile_url_init_fnp url_initfnp, tile_handle_fnp handlefnp, ...);
}

#endif //MAPTILE_DOWNLOAD_H
