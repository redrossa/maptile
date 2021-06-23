//
// Created by @redrossa on 2021-06-20.
//

#ifndef MAPTILE_MAP_H
#define MAPTILE_MAP_H

#include "maptile.h"
#include "tile.h"

#include <iterator>

namespace maptile
{
    class map
    {
    private:
        index_t zoom;
        index_t xmin;
        index_t xmax;
        index_t ymin;
        index_t ymax;
        size_t xshape;
        size_t yshape;
        size_t tile_count;
        tile** tiles;
        index_t t_idx;

    public:
        map(index_t zoom, double lat1, double lon1, double lat2, double lon2);

        ~map();

        index_t get_zoom() const;

        index_t get_xmin() const;

        index_t get_xmax() const;

        index_t get_ymin() const;

        index_t get_ymax() const;

        size_t get_xshape() const;

        size_t get_yshape() const;

        size_t get_tile_count() const;

        degen_tile tile_coords_from_index(index_t i) const;

        index_t tile_coords_to_index(index_t x, index_t y) const;

        index_t tile_coords_to_index(degen_tile t) const;

        void set_tile(tile* t);

        tile* get_tile(index_t i);

        tile* get_tile(index_t x, index_t y);

        size_t expected_size() const;

        void merge_tile_data_to_buf(byte_t* dst, size_t size);

        class iterator
        {
            index_t max;
            index_t itn;
        public:
            iterator(index_t _max, index_t i = 0) : max(_max), itn(i) {};
            iterator& operator++();
            iterator operator++(int);
            bool operator==(iterator other) const;
            bool operator!=(iterator other) const;
            index_t operator*() const;

            using difference_type = index_t;
            using value_type = index_t;
            using pointer = const index_t*;
            using reference = const index_t&;
            using iterator_category = std::input_iterator_tag;
        };

        iterator begin() const;
        iterator end() const;
    };
}

#endif //MAPTILE_MAP_H
