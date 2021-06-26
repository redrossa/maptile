//
// Created by @redrossa on 2021-06-20.
//

#include "../include/transfer.h"

using namespace maptile;

void transfer::append_data(void* chunk, size_t chunk_size)
{
    auto* bytes = static_cast<byte_t*>(chunk);
    data.insert(data.end(), bytes, bytes + chunk_size);
}

index_t transfer::get_id() const
{
    return id;
}

std::vector<byte_t>& transfer::get_data()
{
    return data;
}

void transfer::clear()
{
    data.clear();
}

std::string transfer::get_uri()
{
    return uri;
}

transfer::iterator& transfer::iterator::operator++()
{
    i++;
    return *this;
}

transfer::iterator transfer::iterator::operator++(int)
{
    iterator retval = *this;
    ++(*this);
    return retval;
}

bool transfer::iterator::operator==(transfer::iterator other) const
{
    return i == other.i;
}

bool transfer::iterator::operator!=(transfer::iterator other) const
{
    return !(*this == other);
}

index_t transfer::iterator::operator*() const
{
    return i;
}