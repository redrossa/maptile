//
// Created by @redrossa on 2021-06-20.
//

#include "../include/map.h"

#include <cmath>
#include <stdexcept>

#ifndef M_PI
#   define M_PI 3.1415926535
#endif

#define MAPTILE_MAX_SERIAL_MAP_SIZE 17

using namespace maptile;

static int x_mercator(int zoom, double lon)
{
    size_t tiles = 2 << (zoom - 1);
    double diameter = 2 * M_PI;
    double rad = lon * M_PI / 180.0;
    int x_coord = (int) (tiles * (rad + M_PI) / diameter);
    return x_coord;
}

static int y_mercator(int zoom, double lat)
{
    size_t tiles = 2 << (zoom - 1);
    double diameter = 2 * M_PI;
    double rad = lat * M_PI / 180.0;
    double y_merc = std::log(std::tan(M_PI / 4.0 + rad / 2.0));
    int y_coord = (int) (tiles * (M_PI - y_merc) / diameter);
    return y_coord;
}

map::map(index_t zoom, double lat1, double lon1, double lat2, double lon2)
{
    this->zoom = zoom;

    double lat_min = std::fmax(lat1, lat2);
    double lon_min = std::fmin(lon1, lon2);
    double lat_max = std::fmin(lat1, lat2);
    double lon_max = std::fmax(lon1, lon2);

    xmin = x_mercator(zoom, lon_min);
    xmax = x_mercator(zoom, lon_max);

    ymin = y_mercator(zoom, lat_min);
    ymax = y_mercator(zoom, lat_max);

    xshape = xmax + 1 - xmin;
    yshape = ymax + 1 - ymin;
    tile_count = xshape * yshape;
}

index_t map::get_zoom() const
{
    return zoom;
}

index_t map::get_xmin() const
{
    return xmin;
}

index_t map::get_xmax() const
{
    return xmax;
}

index_t map::get_ymin() const
{
    return ymin;
}

index_t map::get_ymax() const
{
    return ymax;
}

size_t map::get_xshape() const
{
    return xshape;
}

size_t map::get_yshape() const
{
    return yshape;
}

size_t map::get_tile_count() const
{
    return tile_count;
}

tile map::tile_from_index(index_t i) const
{
    if (i >= tile_count)
        throw std::out_of_range("index out of bounds");
    tile t = {zoom, xmin + i % xshape, ymin + i / xshape};
    return t;
}

index_t map::tile_to_index(index_t x, index_t y) const
{
    if (x > xmax || y > ymax)
        throw std::invalid_argument("tile out of bounds");
    return (y - ymin) * xshape + (x - xmin);
}

index_t map::tile_to_index(tile t) const
{
    return tile_to_index(t.x, t.y);
}

static void chop_bytes(std::vector<byte_t>& dst, unsigned long u, size_t size = 0)
{
    if (!size)
        u = sizeof(u);
    std::vector<byte_t> v;
    while (size > 0)
    {
        byte_t b = u & 0xFF;
        v.insert(v.begin(), b);
        u >>= 8;
        size--;
    }
    dst.insert(dst.end(), v.begin(), v.end());
}

std::vector<byte_t> map::to_bytes() const
{
    std::vector<byte_t> b;
    chop_bytes(b, zoom, 1);
    chop_bytes(b, xmin, 4);
    chop_bytes(b, ymin, 4);
    chop_bytes(b, xshape, 4);
    chop_bytes(b, yshape, 4);
    return b;
}
