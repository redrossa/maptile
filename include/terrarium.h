//
// Created by @redrossa on 2021-06-20.
//

#ifndef MAPTILE_TERRARIUM_H
#define MAPTILE_TERRARIUM_H

#include "downloader.h"
#include "map.h"

#include <string>
#include <functional>

namespace terrarium
{
    using namespace maptile;

    std::vector<byte_t> decode(const std::vector<byte_t>& png, unsigned int& w, unsigned int& h);

    class header : public maptile::header
    {
        maptile::tile tile;

    public:
        header(index_t zoom, index_t x, index_t y);

        header(maptile::tile tile) : terrarium::header::header(tile.zoom, tile.x, tile.y) {};

        maptile::tile get_tile() const;
    };

    class yielder
    {
        std::string dir;

    public:
        yielder(const std::string& dir) : dir(dir) {};

        virtual void operator()(const terrarium::header& header, const std::vector<byte_t>& data) const;
    };

    namespace integrium
    {
        class yielder : public terrarium::yielder
        {
        public:
            yielder(const std::string& dir) : terrarium::yielder(dir) {};

            virtual void operator()(const terrarium::header& header, const std::vector<byte_t>& data) const;
        };
    }
}

#endif //MAPTILE_TERRARIUM_H
