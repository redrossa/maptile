//
// Created by @redrossa on 2021-06-20.
//

#ifndef MAPTILE_TILE_H
#define MAPTILE_TILE_H

#include <cstddef>
#include "maptile.h"

namespace maptile
{
    struct degen_tile
    {
        index_t zoom;
        index_t x;
        index_t y;

        degen_tile() : zoom(0), x(0), y(0) {};
        degen_tile(index_t _z, index_t _x, index_t _y) : zoom(_z), x(_x), y(_y) {};

        bool operator==(degen_tile other)
        {
            return zoom == other.zoom && x == other.zoom && y == other.y;
        };

        bool operator!=(degen_tile other)
        {
            return zoom != other.zoom || x != other.zoom || y != other.y;
        };
    };

    class tile : degen_tile
    {
        size_t pixw;
        size_t pixh;
        size_t pix_size;
        byte_t* pix_bytes;
    public:
        tile(index_t zoom, index_t x, index_t y, size_t pixw, size_t pixh, size_t pix_size);

        tile(tile* t);

        ~tile();

        index_t get_zoom() const;

        index_t get_x() const;

        index_t get_y() const;

        size_t get_pixw() const;

        size_t get_pixh() const;

        size_t get_pix_size() const;

        size_t expected_size() const;

        void get_pix(byte_t* dst, index_t xpix, index_t ypix) const;

        void set_pix_bytes(byte_t* src, size_t size);

        byte_t* get_pix_bytes();
    };
}

#endif //MAPTILE_TILE_H
