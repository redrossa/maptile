//
// Created by @redrossa on 6/7/21.
//

#include "../include/map.h"

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

void map_init(map_t * map, int zoom, double lat1, double lon1, double lat2, double lon2)
{
    map->zoom = zoom;

    /* Note that lat/lon values start at vertical center of whole map */
    map->lat_minb = fmax(lat1, lat2);
    map->lon_minb = fmin(lon1, lon2);
    map->lat_maxb = fmin(lat1, lat2);
    map->lon_maxb = fmax(lon1, lon2);

    /* Note that x/y values start at 0 from top-leftmost of whole map */
    map->xminb = x_mercator(zoom, map->lon_minb);
    map->yminb = y_mercator(zoom, map->lat_minb);
    map->xmaxb = x_mercator(zoom, map->lon_maxb);
    map->ymaxb = y_mercator(zoom, map->lat_maxb);
    map->xshape = map->xmaxb + 1 - map->xminb;
    map->yshape = map->ymaxb + 1 - map->yminb;
    map->tile_count = map->xshape * map->yshape;
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
                   map->lat_minb, map->lon_minb,
                   map->lat_maxb, map->lon_maxb,
                   map->xminb, map->xmaxb,
                   map->ymaxb, map->yminb,
                   map->xshape, map->yshape,
                   map->tile_count
    );
    return c;
}
