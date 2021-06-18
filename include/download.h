//
// Created by @redrossa on 6/10/21.
//

#ifndef MAPTILE_DOWNLOAD_H
#define MAPTILE_DOWNLOAD_H

#include <stdio.h>

#include "tile.h"

typedef struct
{
    tile_t * info;
    size_t size;
    unsigned char * data;
} tile_data_t;

char * flush_tile_fname(char *, tile_data_t *, char *);

char * flush_map_fname(char *, map_t *, char *);

#define DOWNLOAD_MAX_PARALLEL 128

int download(map_t *, size_t (*)(tile_data_t *, map_t *, va_list), ...);

int download_multi(map_t *, size_t (*)(tile_data_t *, map_t *, va_list), ...);

#endif //MAPTILE_DOWNLOAD_H