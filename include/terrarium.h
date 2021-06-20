//
// Created by @redrossa on 2021-06-16.
//

#ifndef MAPTILE_TERRARIUM_H
#define MAPTILE_TERRARIUM_H

#include "download.h"

typedef struct
{
    unsigned char * data;
    size_t pixwidth;
    size_t pixheight;
} terrarium_t;

#define TERRARIUM_TILE_DIM 256
#define TERRARIUM_PIXEL_SIZE 3
#define TERRARIUM_PIXEL(terra, x, y) ((x + y * terra->pixwidth) * TERRARIUM_PIXEL_SIZE)
#define TERRARIUM_PIXEL_TOP(terra, x, y) (TERRARIUM_PIXEL(terra, x, y) - terra->pixwidth * TERRARIUM_PIXEL_SIZE)
#define TERRARIUM_PIXEL_BOT(terra, x, y) (TERRARIUM_PIXEL(terra, x, y) + terra->pixwidth * TERRARIUM_PIXEL_SIZE)

terrarium_t * terrarium_init(size_t, size_t);

void terrarium_free(terrarium_t *);

size_t terrarium_tile_fflush(tile_data_t *, map_t *, va_list);

size_t terrarium_map_fflush(tile_data_t *, map_t *, va_list);

int terrarium_jigsaw(terrarium_t *, unsigned int, unsigned int, terrarium_t *);

int terrarium_merge(terrarium_t *, terrarium_t **, size_t, size_t);

terrarium_t * terrarium_init_decode(unsigned char *png_bytes, size_t size);

int terrarium_get_pixel(terrarium_t *, int, int, unsigned char *);

int terrarium_get_top_pixel(terrarium_t *, int, int, unsigned char *);

int terrarium_get_bottom_pixel(terrarium_t *, int, int, unsigned char *);

#endif //MAPTILE_TERRARIUM_H
