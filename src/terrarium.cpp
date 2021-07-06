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

//transfer::iterator builder::begin() const
//{
//    return transfer::iterator();
//}
//
//transfer::iterator builder::end() const
//{
//    return transfer::iterator(m.get_tile_count());
//}
//
//transfer* builder::build_transfer(transfer::iterator i) const
//{
//    index_t id = *i;
//    tile t = m.tile_from_index(id);
//    std::ostringstream oss;
//    oss << TERRARIUM_ENDPOINT << "/" << t.zoom << "/" << t.x << "/" << t.y << ".png";
//    return new transfer(id, oss.str());
//}
//
std::vector<byte_t> terrarium::decode(const std::vector<byte_t>& png, unsigned int& w, unsigned int& h)
{
    std::vector<byte_t> image; // raw pixels
    unsigned int error = lodepng::decode(image, w, h, png, LCT_RGB);
    if (error) throw std::exception();
    return image;
}
//
//stratorium_writer::stratorium_writer(const map& m) : m(m)
//{
//    size_t size = m.get_tile_count() * 256 * 256;
//    rgb[0].reserve(size);
//    rgb[1].reserve(size);
//    rgb[2].reserve(size);
//}
//
//void stratorium_writer::operator()(transfer* t)
//{
//    std::vector<byte_t> raw = decode(t->get_data());
//    int i = 0;
//    for (auto& b : raw)
//    {
//        std::vector<byte_t> v = rgb[i++ % 3];
//        v.insert(v.begin() + (i / (256 * 3) * (m.get_tile_count() * 256 * 3)), b);
//    }
//}

terrarium::header::header(index_t zoom, index_t x, index_t y) : tile{zoom, x, y}, maptile::header("")
{
    std::ostringstream oss;
    oss << TERRARIUM_ENDPOINT << "/" << zoom << "/" << x << "/" << y << ".png";
    address = oss.str();
}

maptile::tile terrarium::header::get_tile() const
{
    return tile;
}

void yielder::operator()(const terrarium::header& header, const std::vector<byte_t>& data) const
{
    tile t = header.get_tile();
    std::ostringstream fname;
    fname << dir << "/" << t.zoom << "-" << t.x << "-" << t.y << ".png";
    std::ofstream fout(fname.str(), std::ios::out | std::ios::binary);
    if (!fout.is_open())
        throw std::ios_base::failure("Cannot open file " + fname.str());
    fout.write((const char *) &data[0], data.size() * sizeof(byte_t));
}

void integrium::yielder::operator()(const terrarium::header& header, const std::vector<byte_t>& data) const
{
    unsigned int w, h;
    std::vector<byte_t> decoded = decode(data, w, h);

    std::vector<byte_t> integral;
    integral.reserve(decoded.size() / 3);
    size_t dec_size = decoded.size();
    for (index_t i = 0; i < dec_size; i++)
        if (i % 3 != 2)
            integral.push_back(decoded[i]);

    std::vector<byte_t> integral_png;
    lodepng::encode(integral_png, integral, w, h, LCT_GREY_ALPHA);

    terrarium::yielder::operator()(header, integral_png);
}
