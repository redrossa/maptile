//
// Created by Muhammad Adriano Raksi on 6/10/21.
//

#include "../include/tile.h"

#include <stdio.h>
#include <string.h>

void tile_iterator(tile_t * tile, map_t * map)
{
    tile->zoom = map->zoom;
    tile->x = map->xminb;
    tile->y = map->yminb;
}

int tile_next(tile_t * tile, map_t * map)
{
    if (tile->zoom != map->zoom
        || tile->x > map->xmaxb
        || tile->y > map->ymaxb
        || tile->x == map->xmaxb && tile->y == map->ymaxb)
        return 0;
    if (tile->y < map->ymaxb)
        tile->y++;
    else if (tile->x < map->xmaxb)
    {
        tile->x++;
        tile->y = map->yminb;
    }
    return 1;
}

int tile_fromindex(tile_t * tile, map_t * map, int i)
{
    if (i >= map->tile_count)
        return 0;
    tile->zoom = map->zoom;
    tile->x = map->xminb + i / map->yshape;
    tile->y = map->yminb + i % map->yshape;
    return 1;
}

int tile_toindex(tile_t * tile, map_t * map)
{
    if (tile->zoom != map->zoom
        || tile->x > map->xmaxb
        || tile->y > map->ymaxb)
        return -1;
    return (tile->x - map->xminb) * map->yshape;
}

size_t len_num(unsigned int n)
{
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

int tile_tostring(tile_t * tile, char * dest)
{
    int length = 0;

    length += sprintf(dest, "%d", tile->zoom);
    length += sprintf(dest + length, "-");
    length += sprintf(dest + length, "%d", tile->x);
    length += sprintf(dest + length, "-");
    length += sprintf(dest + length, "%d", tile->y);

    return length;
}