//
// Created by Reno on 2021-06-25.
//

#include "../include/downloader.h"

#include <sstream>
#include <fstream>

using namespace maptile;

void downloader::fwriter::operator()(transfer* t) const
{
    tile tile = m.tile_from_index(t->get_id());
    std::ostringstream oss;
    oss << dirname << "/" << tile.zoom << "-" << tile.x << "-" << tile.y << "." << ext;
    std::ofstream fs(oss.str(), std::ios::out | std::ios::binary);
    if (!fs) throw std::exception();
    std::copy(t->get_data().cbegin(), t->get_data().cend(), std::ostream_iterator<unsigned char>(fs));
    fs.close();
}