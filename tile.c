//
// Created by Muhammad Adriano Raksi on 6/10/21.
//

#include "tile.h"

#include <stdio.h>
#include <string.h>

void tile_iterator(map_t * map, tile_t * current)
{
    current->zoom = map->zoom;
    current->x = map->xmin;
    current->y = map->ymin;
}

int tile_next(map_t * map, tile_t * current)
{
    if (current->zoom != map->zoom
        || current->x > map->xmax
        || current->y > map->ymax
        || current->x == map->xmax && current->y == map->ymax)
        return 0;
    if (current->y < map->ymax)
        current->y++;
    else if (current->x < map->xmax)
    {
        current->x++;
        current->y = map->ymin;
    }
    return 1;
}

int tile_fromindex(map_t * map, tile_t * tile, int i)
{
    if (i >= map->tile_count)
        return 0;
    tile->zoom = map->zoom;
    tile->x = map->xmin + i / map->yshape;
    tile->y = map->ymin + i % map->yshape;
    return 1;
}

#define MAPTILE_ENDPOINT "https://s3.amazonaws.com/elevation-tiles-prod/terrarium/"
#define MAPTILE_ENDPOINT_LENGTH 56

size_t len_num(unsigned int n) {
    int len = 1;
    while (n > 9)
    {
        len++;
        n /= 10;
    }
    return len;
}

int tile_url(tile_t * tile, char * dest)
{
    int len = 0;
    char tmp[6];

    // copy the endpoint
    strcpy(dest, MAPTILE_ENDPOINT);
    len += MAPTILE_ENDPOINT_LENGTH;


    // concat the zoom value
    sprintf(tmp, "%d", tile->zoom);
    strcat(dest, tmp);
    len += len_num(tile->zoom);

    // concat the url separator
    strcat(dest, "/");
    len++;

    // concat the x value
    sprintf(tmp, "%d", tile->x);
    strcat(dest, tmp);
    len += len_num(tile->x);

    // concat the url separator
    strcat(dest, "/");
    len++;

    // concat the y value
    sprintf(tmp, "%d", tile->y);
    strcat(dest, tmp);
    len += len_num(tile->y);

    // concat the file extension
    strcat(dest, ".png");
    len += 4;

    return len;
}