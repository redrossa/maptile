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

#define URL_PATH_SEPARATOR 1
#define PNG_EXT 4

static char * build_url(int zoom, int x, int y)
{
    char * s = malloc(strlen(MAPTILE_ENDPOINT) +
            len_num(zoom) + URL_PATH_SEPARATOR +
            len_num(x) + URL_PATH_SEPARATOR +
            len_num(y) + PNG_EXT + 1);

    if (!s)
        return NULL;

    char tmp[6];

    // copy the endpoint
    strcpy(s, MAPTILE_ENDPOINT);

    // concat the zoom value
    sprintf(tmp, "%d", zoom);
    strcat(s, tmp);

    // concat the url separator
    strcat(s, "/");

    // concat the x value
    sprintf(tmp, "%d", x);
    strcat(s, tmp);

    // concat the url separator
    strcat(s, "/");

    // concat the y value
    sprintf(tmp, "%d", y);
    strcat(s, tmp);

    // concat the file extension
    strcat(s, ".png");

    return s;
}

char ** map_get_urls(map_t * map)
{
    char ** urls = malloc(sizeof(char *) * map->tile_count);

    if (!urls)
        return NULL;

    for (int i = 0; i < map->tile_count; i++)
    {
        int x = map->xmin + i / map->yshape;
        int y = map->ymin + i % map->yshape;
        char * tmp = build_url(map->zoom, x, y);

        if (!tmp)
        {
            for (int j = 0; j < i; j++)
                free(urls[j]);
            free(urls);
            return NULL;
        }

        urls[i] = tmp;
    }

    return urls;
}
