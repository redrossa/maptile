//
// Created by @redrossa on 2021-06-20.
//

#ifndef MAPTILE_TERRARIUM_H
#define MAPTILE_TERRARIUM_H

#include "download.h"

namespace terrarium {
    using namespace maptile;

    class builder : public transfer::builder
    {
        using transfer::builder::builder;

    public:
        transfer* operator()(transfer::iterator i) override;
    };
}

#endif //MAPTILE_TERRARIUM_H
