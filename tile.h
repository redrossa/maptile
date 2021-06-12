//
// Created by Muhammad Adriano Raksi on 6/10/21.
//

#ifndef MAPTILE_TILE_H
#define MAPTILE_TILE_H

#include "map.h"

typedef struct
{
    int zoom;
    int x;
    int y;
} tile_t;

void tile_iterator(tile_t *, map_t *);

int tile_next(tile_t *, map_t *);

int tile_fromindex(tile_t *, map_t *, int);

int tile_url(tile_t *, char *);

#endif //MAPTILE_TILE_H
