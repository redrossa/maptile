//
// Created by @redrossa on 2021-06-25.
//

#ifndef MAPTILE_TRANSFER_H
#define MAPTILE_TRANSFER_H

#include "map.h"

#include <string>
#include <vector>
#include <functional>

namespace maptile
{
    class transfer
    {
        index_t id;
        std::string uri;
        std::vector<byte_t> data;

    public:
        transfer(index_t id, std::string uri) : id(id), uri(uri) {};

        virtual ~transfer() = default;

        void append_data(void* chunk, size_t chunk_size);

        index_t get_id() const;

        std::vector<byte_t>& get_data();

        void clear();

        std::string get_uri();

        class iterator
        {
            index_t i;

        public:
            iterator(index_t i = 0) : i(i) {};

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

        class builder
        {
        protected:
            const map& m;

        public:
            explicit builder(const map& m) : m(m) {};

            iterator begin() const;

            iterator end() const;

            virtual transfer* operator()(iterator i) = 0;
        };

        typedef std::function<transfer*(index_t)> buildfn;

        typedef std::function<void(transfer*)> yieldfn;
    };
}

#endif //MAPTILE_TRANSFER_H
