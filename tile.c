//
// Created by Muhammad Adriano Raksi on 6/10/21.
//

#include "tile.h"

#include <stdio.h>
#include <string.h>

void tile_iterator(tile_t * tile, map_t * map)
{
    tile->zoom = map->zoom;
    tile->x = map->xmin;
    tile->y = map->ymin;
}

int tile_next(tile_t * tile, map_t * map)
{
    if (tile->zoom != map->zoom
        || tile->x > map->xmax
        || tile->y > map->ymax
        || tile->x == map->xmax && tile->y == map->ymax)
        return 0;
    if (tile->y < map->ymax)
        tile->y++;
    else if (tile->x < map->xmax)
    {
        tile->x++;
        tile->y = map->ymin;
    }
    return 1;
}

int tile_fromindex(tile_t * tile, map_t * map, int i)
{
    if (i >= map->tile_count)
        return 0;
    tile->zoom = map->zoom;
    tile->x = map->xmin + i / map->yshape;
    tile->y = map->ymin + i % map->yshape;
    return 1;
}

size_t len_num(unsigned int n) {
    int len = 1;
    while (n > 9)
    {
        len++;
        n /= 10;
    }
    return len;
}

#define MAPTILE_ENDPOINT "https://s3.amazonaws.com/elevation-tiles-prod/terrarium/"

int tile_url(tile_t * tile, char * dest)
{
    int length = 0;

    length += sprintf(dest, MAPTILE_ENDPOINT);
    length += sprintf(dest + length, "%d", tile->zoom);
    length += sprintf(dest + length, "/");
    length += sprintf(dest + length, "%d", tile->x);
    length += sprintf(dest + length, "/");
    length += sprintf(dest + length, "%d", tile->y);
    length += sprintf(dest + length, ".png");

    return length;
}