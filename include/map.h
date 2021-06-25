//
// Created by @redrossa on 2021-06-20.
//

#ifndef MAPTILE_MAP_H
#define MAPTILE_MAP_H

#include "maptile.h"

#include <iterator>

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
    };
}

#endif //MAPTILE_MAP_H
