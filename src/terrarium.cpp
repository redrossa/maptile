//
// Created by @redrossa on 2021-06-20.
//

#include "../include/terrarium.h"
#include "../include/lodepng.h"

#include <sstream>
#include <vector>

using namespace terrarium;
using namespace maptile;

#define TERRARIUM_ENDPOINT "https://s3.amazonaws.com/elevation-tiles-prod/terrarium"

transfer* builder::operator()(transfer::iterator i)
{
    index_t id = *i;
    tile t = m.tile_from_index(id);
    std::ostringstream oss;
    oss << TERRARIUM_ENDPOINT << "/" << t.zoom << "/" << t.x << "/" << t.y << ".png";
    return new transfer(id, oss.str());
}

std::vector<byte_t> terrarium::decode(const std::vector<byte_t>& png)
{
    std::vector<byte_t> image; //the raw pixels
    unsigned width, height;
    unsigned int error = lodepng::decode(image, width, height, png);
    if (error) throw std::exception();
    return image;
}