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

map_t * map_new(int, double, double, double, double);

void map_del(map_t *);

int map_pprint(map_t *);

#endif //MAPTILE_MAP_H
