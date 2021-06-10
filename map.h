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

typedef struct
{
    int zoom;
    int x;
    int y;
} tile_t;

void tile_iterator(map_t *, tile_t *);

int tile_next(map_t *, tile_t *);

int tile_url(tile_t *, char *);

map_t * map_new(int, double, double, double, double);

void map_del(map_t *);

int map_pprint(map_t *);

char ** map_get_urls(map_t *map);

#endif //MAPTILE_MAP_H
