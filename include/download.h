//
// Created by @redrossa on 2021-06-20.
//

#include "map.h"

#include <curl/curl.h>

#ifndef MAPTILE_DOWNLOAD_H
#define MAPTILE_DOWNLOAD_H

#include <concepts>
#include <functional>

namespace maptile
{
    class transfer
    {
        index_t i;
        byte_t* data;
        size_t size;

    public:
        transfer(index_t i);

        ~transfer();

        void append_data(void* chunk, size_t chunk_size);

        void get_data(byte_t* dst);

        virtual void get_url(char* dst) = 0;
    };

    typedef std::function<transfer*(maptile::index_t)> transfer_builder;

    typedef std::function<int(index_t, transfer*)> transfer_handler;

    template<typename T>
    int download(const transfer_builder& build,
                 const transfer_handler& handler,
                 size_t nparallel = 128);

    typedef struct
    {
        index_t z;
        index_t x;
        index_t y;
        byte_t* data;
        size_t size;
    } tile_transfer_t;

    typedef char* (* tile_url_init_fnp)(map* m, index_t x, index_t y);

    typedef int (* tile_handle_fnp)(index_t itc, map* m, tile_transfer_t* tile_transf, va_list args);

    int verbose_flush(index_t itc, map* m, tile_transfer_t* tile_transf, va_list args);

    int download_tiles(map* m, tile_url_init_fnp url_initfn, tile_handle_fnp handlefn, ...);
}

#endif //MAPTILE_DOWNLOAD_H
