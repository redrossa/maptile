//
// Created by @redrossa on 2021-06-20.
//

#include "../include/tile.h"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <new>

using namespace maptile;

tile::tile(index_t zoom, index_t x, index_t y, size_t pixw, size_t pixh, size_t pix_size) : degen_tile(zoom, x, y)
{
    this->pixw = pixw;
    this->pixh = pixh;
    this->pix_size = pix_size;
    this->pix_bytes = nullptr;
}

tile::tile(tile* t) : degen_tile(t->zoom, t->x, t->y)
{
    pixw = t->pixw;
    pixh = t->pixh;
    pix_size = t->pix_size;
    pix_bytes = nullptr;
    if (t->pix_bytes) {
        pix_bytes = static_cast<byte_t*>(malloc(sizeof(byte_t) * t->expected_size()));
        if (!pix_bytes)
            throw std::bad_alloc();
        memcpy(pix_bytes, t->pix_bytes, t->expected_size());
    }
}

tile::~tile()
{
    std::free(pix_bytes);
}

index_t tile::get_zoom() const
{
    return zoom;
}

index_t tile::get_x() const
{
    return x;
}

index_t tile::get_y() const
{
    return y;
}

size_t tile::get_pixw() const
{
    return pixw;
}

size_t tile::get_pixh() const
{
    return pixh;
}

size_t tile::get_pix_size() const
{
    return pix_size;
}

size_t tile::expected_size() const
{
    return pixw * pixh * pix_size;
}

void tile::get_pix(byte_t* dst, index_t xpix, index_t ypix) const
{
    if (!pix_bytes)
        throw std::exception();
    std::memcpy(dst, &pix_bytes[(x + y * pixw) * pix_size], pix_size);
}

void tile::set_pix_bytes(byte_t* src, size_t size)
{
    free(pix_bytes); /* Free previous value, if any */
    if (size != expected_size())
        throw std::length_error("incorrect source size");
    pix_bytes = src;
}

byte_t* tile::get_pix_bytes()
{
    return pix_bytes;
}
