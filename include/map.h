//
// Created by @redrossa on 6/7/21.
//

#ifndef MAPTILE_MAP_H
#define MAPTILE_MAP_H

#define MAPTILE_TILE_DIM 256

typedef struct
{
    /* Zoom of this map */
    int zoom;

    /* Minimum bound is the top-leftmost bound */
    double lat_minb;
    double lon_minb;

    /* Maximum bound is the low-rightmost bound */
    double lat_maxb;
    double lon_maxb;

    /* X of the minimum bounding tile */
    int xminb;

    /* Y of the minimum bounding tile */
    int yminb;

    /* X of the maximum bounding tile */
    int xmaxb;

    /* Y of the maximum bounding tile */
    int ymaxb;

    /* Number of tiles horizontally */
    int xshape;

    /* Number of tiles vertically */
    int yshape;

    /* Total number of tiles */
    int tile_count;
} map_t;

void map_init(map_t *, int, double, double, double, double);

int map_pprint(map_t *);

#endif //MAPTILE_MAP_H
