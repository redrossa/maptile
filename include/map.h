//
// Created by @redrossa on 2021-06-20.
//

#ifndef MAPTILE_MAP_H
#define MAPTILE_MAP_H

#include "maptile.h"

#include <vector>

namespace maptile
{
    struct tile
    {
        index_t zoom;
        index_t x;
        index_t y;
    };

    class map
    {
    protected:
        index_t zoom;
        index_t xmin;
        index_t xmax;
        index_t ymin;
        index_t ymax;
        size_t xshape;
        size_t yshape;
        size_t tile_count;

    public:
        map(index_t zoom, double lat1, double lon1, double lat2, double lon2);

        index_t get_zoom() const;

        index_t get_xmin() const;

        index_t get_xmax() const;

        index_t get_ymin() const;

        index_t get_ymax() const;

        size_t get_xshape() const;

        size_t get_yshape() const;

        size_t get_tile_count() const;

        tile tile_from_index(index_t i) const;

        index_t tile_to_index(index_t x, index_t y) const;

        index_t tile_to_index(tile t) const;

        std::vector<byte_t> to_bytes() const;

        class tile_iterator
        {
            const map& m;
            index_t i;

        public:
            tile_iterator(const map& m, index_t i = 0) : m(m), i(i) {};

            bool operator==(const tile_iterator& other) const;

            bool operator!=(const tile_iterator& other) const;

            tile operator*() const;

            tile_iterator& operator++();

            tile_iterator operator++(int);
        };

        tile_iterator begin() const;

        tile_iterator end() const;
    };
}

#endif //MAPTILE_MAP_H
