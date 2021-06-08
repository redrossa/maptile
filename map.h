//
// Created by @redrossa on 6/7/21.
//

#ifndef MAPTILE_MAP_H
#define MAPTILE_MAP_H

#define MAPTILE_TILE_DIM 256

typedef struct maptile_map
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
} maptile_map_t;

maptile_map_t * maptile_map_new(int, double, double, double, double);

void maptile_map_del(maptile_map_t *);

int maptile_pprint(maptile_map_t *);

char ** maptile_build_urls(maptile_map_t *);

#endif //MAPTILE_MAP_H
