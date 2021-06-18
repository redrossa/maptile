//
// Created by @redrossa on 2021-06-16.
//

#ifndef MAPTILE_TERRARIUM_H
#define MAPTILE_TERRARIUM_H

#include "download.h"

typedef struct
{
    unsigned char ** rows;
    size_t pixwidth;
    size_t pixheight;
} terrarium_t;

#define TERRARIUM_PIXEL_SIZE 3

size_t terrarium_tile_flush(tile_data_t *, va_list);

size_t terrarium_heights(unsigned char *, size_t, terrarium_t *);

#endif //MAPTILE_TERRARIUM_H
