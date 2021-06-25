//
// Created by @redrossa on 2021-06-20.
//

#include "../include/terrarium.h"

using namespace terrarium;
using namespace maptile;

#define TERRARIUM_ENDPOINT "https://s3.amazonaws.com/elevation-tiles-prod/terrarium/%d/%d/%d.png"

transfer* builder::operator()(transfer::iterator i)
{
    index_t id = *i;
    tile t = m.tile_from_index(id);
    char url[256];
    sprintf(url, TERRARIUM_ENDPOINT, t.zoom, t.x, t.y);
    return new transfer(id, std::string(url));
}
