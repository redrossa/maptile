//
// Created by @redrossa on 6/10/21.
//

#ifndef MAPTILE_DOWNLOAD_H
#define MAPTILE_DOWNLOAD_H

#include <stdio.h>

#include "map.h"
#include "tile.h"

int download(map_t * map, size_t (*)(unsigned char *, size_t, tile_t *, int, va_list), ...);

int download_multi(char *, map_t *);

#endif //MAPTILE_DOWNLOAD_H
