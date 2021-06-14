//
// Created by @redrossa on 6/7/21.
//

#ifndef MAPTILE_MAP_H
#define MAPTILE_MAP_H

#define MAPTILE_TILE_DIM 256

typedef struct
{
    int zoom;
    double lat_minbound;
    double lon_minbound;
    double lat_maxbound;
    double lon_maxbound;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    int xshape;
    int yshape;
    int tile_count;
} map_t;

void map_init(map_t *, int, double, double, double, double);

int map_pprint(map_t *);

#endif //MAPTILE_MAP_H
