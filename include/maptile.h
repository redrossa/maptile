//
// Created by @redrossa on 2021-06-20.
//

#ifndef MAPTILE_MAPTILE_H
#define MAPTILE_MAPTILE_H

#include <stdio.h>

namespace maptile {
    typedef size_t index_t;
    typedef unsigned char byte_t;

    enum errc {
        ERRC_OK = 0,
        ERRC_MEM_ALLOC = 1,
        ERRC_CURL = 2,
    };
}

#endif //MAPTILE_MAPTILE_H
