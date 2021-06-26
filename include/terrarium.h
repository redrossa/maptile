//
// Created by @redrossa on 2021-06-20.
//

#ifndef MAPTILE_TERRARIUM_H
#define MAPTILE_TERRARIUM_H

#include "transfer.h"

namespace terrarium
{
    using namespace maptile;

    class builder : public transfer::builder
    {
        using transfer::builder::builder;

    public:
        transfer* operator()(transfer::iterator i) override;
    };

    std::vector<byte_t> decode(const std::vector<byte_t>& png);
}

#endif //MAPTILE_TERRARIUM_H
