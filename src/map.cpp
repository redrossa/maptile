//
// Created by @redrossa on 2021-06-20.
//

#include "../include/map.h"

#include <string>
#include <cmath>
#include <stdexcept>

#ifndef M_PI
#   define M_PI 3.1415926535
#endif

using namespace maptile;

static int x_mercator(int zoom, double lon) {
    size_t tiles = 2 << (zoom - 1);
    double diameter = 2 * M_PI;
    double rad = lon * M_PI / 180.0;
    int x_coord = (int) (tiles * (rad + M_PI) / diameter);
    return x_coord;
}

static int y_mercator(int zoom, double lat) {
    size_t tiles = 2 << (zoom - 1);
    double diameter = 2 * M_PI;
    double rad = lat * M_PI / 180.0;
    double y_merc = std::log(std::tan(M_PI / 4.0 + rad / 2.0));
    int y_coord = (int) (tiles * (M_PI - y_merc) / diameter);
    return y_coord;
}

map::map(index_t zoom, double lat1, double lon1, double lat2, double lon2) {
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

    tiles = nullptr;
    t_idx = tile_count;
}

map::~map() {
    if (!tiles)
        return;
    for (int i = 0; i < tile_count; ++i)
        delete tiles[i];
    free(tiles);
}

index_t map::get_zoom() const {
    return zoom;
}

index_t map::get_xmin() const {
    return xmin;
}

index_t map::get_xmax() const {
    return xmax;
}

index_t map::get_ymin() const {
    return ymin;
}

index_t map::get_ymax() const {
    return ymax;
}

size_t map::get_xshape() const {
    return xshape;
}

size_t map::get_yshape() const {
    return yshape;
}

size_t map::get_tile_count() const {
    return tile_count;
}

void map::tile_coord_from_index(index_t* xdst, index_t* ydst, index_t i) const {
    if (i >= tile_count)
        throw std::out_of_range("index out of bounds");
    *xdst = xmin + i % xshape;
    *ydst = ymin + i / xshape;
}

index_t map::tile_coord_to_index(index_t x, index_t y) const {
    if (x > xmax || y > ymax)
        throw std::invalid_argument("tile out of bounds");
    return (y - ymin) * xshape + (x - xmin);
}

void map::set_tile(tile* t) {
    if (t->get_zoom() != zoom || t->get_x() > xmax || t->get_y() > ymax)
        throw std::invalid_argument("tile out of bounds");
    if (t_idx < tile_count && t->expected_size() != tiles[t_idx]->expected_size())
        throw std::invalid_argument("tile dimension must match with other tiles");

    if (!tiles)
        tiles = new tile* [tile_count]{nullptr};
    index_t i = tile_coord_to_index(t->get_x(), t->get_y());
    delete tiles[i];
    tiles[i] = t;
    t_idx = i;
}

tile* map::get_tile(index_t i) {
    if (!tiles)
        throw std::exception();
    return tiles[i];
}

tile* map::get_tile(index_t x, index_t y) {
    return get_tile(tile_coord_to_index(x, y));
}

size_t map::expected_size() const {
    return t_idx ? tiles[t_idx]->expected_size() * tile_count : 0;
}

void map::merge_tile_data_to_buf(byte_t* dst, size_t size) {
    if (!tiles)
        throw std::exception();
    if (size < expected_size())
        throw std::exception();
    size_t dst_pixw = xshape * tiles[t_idx]->get_pixw();
    for (int i = 0; i < tile_count; i++) {
        tile* t = tiles[i];

        index_t dst_byte_x = i % xshape * t->get_pixw();
        index_t dst_byte_y = i / xshape * t->get_pixh();
        index_t dst_start = (dst_byte_x + dst_byte_y * dst_pixw) * t->get_pix_size();

        for (int r = 0; r < t->get_pixh(); r++) {
            index_t dst_row_start = dst_start + r * dst_pixw * t->get_pix_size();
            index_t src_row_start = r * t->get_pixw() * t->get_pix_size();
            memcpy(&dst[dst_row_start], &t->get_pix_bytes()[src_row_start], t->get_pixw() * t->get_pix_size());
        }
    }
}