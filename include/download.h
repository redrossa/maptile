//
// Created by @redrossa on 2021-06-20.
//

#include "map.h"

#include <curl/curl.h>

#ifndef MAPTILE_DOWNLOAD_H
#define MAPTILE_DOWNLOAD_H

namespace maptile {
    typedef struct {
        index_t z;
        index_t x;
        index_t y;
        byte_t* data;
        size_t size;
    } tile_transfer_t;

    typedef char* tile_url_initfn_t(map* m, index_t x, index_t y);

    typedef int tile_handlefn_t(index_t itc, map* m, tile_transfer_t* tile_transf, va_list args);

    int download_tiles(map* m, tile_url_initfn_t* url_initfnp, tile_handlefn_t* handlefnp, ...);
}

#endif //MAPTILE_DOWNLOAD_H
