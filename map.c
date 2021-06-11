//
// Created by @redrossa on 6/7/21.
//

#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#   define M_PI 3.1415926535
#endif

static int x_mercator(int zoom, double lon)
{
    size_t tiles = 2 << (zoom - 1);
    double diameter = 2 * M_PI;
    double rad = lon * M_PI / 180.0;
    int x_coord = (int) (tiles * (rad + M_PI)/diameter);
    return x_coord;
}

static int y_mercator(int zoom, double lat)
{
    size_t tiles = 2 << (zoom - 1);
    double diameter = 2 * M_PI;
    double rad = lat * M_PI / 180.0;
    double y_merc = log(tan(M_PI / 4.0 + rad / 2.0));
    int y_coord = (int) (tiles * (M_PI - y_merc)/diameter);
    return y_coord;
}

map_t * map_new(int zoom, double lat1, double lon1, double lat2, double lon2)
{
    map_t * map = malloc(sizeof(map_t));

    if (!map)
        return NULL;

    map->zoom = zoom;
    map->lat_minbound = fmax(lat1, lat2);
    map->lon_minbound = fmin(lon1, lon2);
    map->lat_maxbound = fmin(lat1, lat2);
    map->lon_maxbound = fmax(lon1, lon2);
    map->xmin = x_mercator(zoom, map->lon_minbound);
    map->ymin = y_mercator(zoom, map->lat_minbound);
    map->xmax = x_mercator(zoom, map->lon_maxbound);
    map->ymax = y_mercator(zoom, map->lat_maxbound);
    map->xshape = map->xmax + 1 - map->xmin;
    map->yshape = map->ymax + 1 - map->ymin;
    map->tile_count = map->xshape * map->yshape;

    return map;
}

void map_del(map_t * map)
{
    free(map);
}

int map_pprint(map_t * map) {
    int c = printf("{\n"
                   "\tzoom: %d,\n"
                   "\tminbound: (%lf, %lf),\n"
                   "\tmaxbound: (%lf, %lf),\n"
                   "\txrange: (%d, %d),\n"
                   "\tyrange: (%d, %d),\n"
                   "\tshape: (%d, %d),\n"
                   "\ttile_count: %d\n"
                   "}\n",
                   map->zoom,
                   map->lat_minbound, map->lon_minbound,
                   map->lat_maxbound, map->lon_maxbound,
                   map->xmin, map->xmax,
                   map->ymin, map->ymax,
                   map->xshape, map->yshape,
                   map->tile_count
    );
    return c;
}
