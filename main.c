#include <stdio.h>
#include "map.h"

void urls(maptile_map_t * map)
{
    for (int x = map->xmin; x <= map->xmax; x++)
        for (int y = map->ymin; y <= map->ymax; y++)
            printf("(%d, %d, %d)\n", map->zoom, x, y);
}

int main()
{
    maptile_map_t * map = maptile_map_new(15, 46.49093558159459, 6.1789037495878345, 43.71564665363514, 13.69355223256804);
    maptile_pprint(map);
    maptile_map_del(map);
    return 0;
}
