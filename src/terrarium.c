//
// Created by @redrossa on 2021-06-16.
//

#include "../include/terrarium.h"

#include "../include/spng.h"
#include "../include/lodepng.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

terrarium_t * terrarium_init(size_t pixw, size_t pixh)
{
    terrarium_t * t = malloc(sizeof(terrarium_t));
    if (!t)
        return NULL;
    t->data = malloc(sizeof(unsigned char) * pixh * pixw * TERRARIUM_PIXEL_SIZE);
    if (!t->data)
    {
        free(t);
        return NULL;
    }
    t->pixwidth = pixw;
    t->pixheight = pixh;
    return t;
}

void terrarium_free(terrarium_t * t)
{
    free(t->data);
    free(t);
}

size_t terrarium_tile_fflush(tile_data_t * td, map_t * map, va_list args)
{
    char * dirname = va_arg(args, char *);
    char * fname = flush_tile_fname(dirname, td, "terrarium.png");
    if (!fname)
        return 0;

    FILE * fp = fopen(fname, "wb");
    if (!fp)
    {
        free(fname);
        return 0;
    }

    size_t res = fwrite(td->data, sizeof(*td->data), td->size, fp);

    fclose(fp);
    free(fname);
    return res;
}

int terrarium_jigsaw(terrarium_t * dst, unsigned int xpix, unsigned int ypix, terrarium_t * src)
{
    if (dst->pixwidth < xpix + src->pixwidth || dst->pixheight < ypix + src->pixheight)
        return 0;
    unsigned int dst_start = TERRARIUM_PIXEL(dst, xpix, ypix);
    for (int i = 0; i < src->pixheight; i++)
    {
        unsigned int dst_row_start = dst_start + i * dst->pixwidth * TERRARIUM_PIXEL_SIZE;
        unsigned int src_row_start = i * src->pixwidth * TERRARIUM_PIXEL_SIZE;
        memcpy(&dst->data[dst_row_start], &src->data[src_row_start], src->pixwidth * TERRARIUM_PIXEL_SIZE);
    }
    return 1;
}

int terrarium_merge(terrarium_t * dst, terrarium_t ** srcs, size_t xcount, size_t ycount)
{
    size_t tile_count = xcount * ycount;
    size_t tile_width = (srcs[0])->pixwidth;
    size_t tile_height = (srcs[0])->pixheight;
    for (int i = 0; i < tile_count; i++)
    {
        if (srcs[i]->pixwidth != tile_width || srcs[i]->pixheight != tile_height)
            return 0;
        unsigned int map_byte_x = i % xcount * tile_width;
        unsigned int map_byte_y = i / xcount * tile_height;
        if (!terrarium_jigsaw(dst, map_byte_x, map_byte_y, srcs[i]))
            return 0;
    }

    return 1;
}

static void free_merger_holder(terrarium_t ** holder, int n)
{
    for (int i = 0; i < n; i++)
        if (holder[i])
            terrarium_free(holder[i]);
    free(holder);
}

size_t terrarium_map_fflush(tile_data_t * td, map_t * map, va_list args)
{
    static int itc = 0;
    static terrarium_t ** holder = NULL;

    if (!itc++)
    {
        holder = calloc(map->tile_count, sizeof(terrarium_t *));
        if (!holder)
            return 0;
    }

    if (holder)
    {
        int current_tile_index = tile_toindex(td->info, map);
        terrarium_t * t = terrarium_init_decode(td->data, td->size);
        if (!t)
        {
            free_merger_holder(holder, map->tile_count);
            holder = NULL;
            return 0;
        }
        holder[current_tile_index] = t;

        /* Only at the last iteration */
        if (itc == map->tile_count)
        {
            itc = 0;
            terrarium_t * merged = terrarium_init(map->xshape * TERRARIUM_TILE_DIM, map->yshape * TERRARIUM_TILE_DIM);
            if (!merged)
            {
                free_merger_holder(holder, map->tile_count);
                holder = NULL;
                return 0;
            }
            if (!terrarium_merge(merged, holder, map->xshape, map->yshape))
            {
                free_merger_holder(holder, map->tile_count);
                terrarium_free(merged);
                holder = NULL;
                return 0;
            }

            char * dirname = va_arg(args, char *);
            char * fname = flush_map_fname(dirname, map, "terrarium.png");
            if (!fname)
            {
                free_merger_holder(holder, map->tile_count);
                terrarium_free(merged);
                holder = NULL;
                return 0;
            }

            lodepng_encode24_file(fname, merged->data, merged->pixwidth, merged->pixheight);
            holder = NULL;
        }
    }

    return td->size;
}

terrarium_t * terrarium_init_decode(unsigned char * png_bytes, size_t size)
{
    /* Initialize spng context */
    spng_ctx * ctx = spng_ctx_new(0);
    if (!ctx)
    {
        return NULL;
    }

    /* Set context to the buffer containing the terrarium data encoded in png */
    if (spng_set_png_buffer(ctx, png_bytes, size))
    {
        spng_ctx_free(ctx);
        return NULL;
    }

    /* Retrieve IHDR */
    struct spng_ihdr ihdr;
    if (spng_get_ihdr(ctx, &ihdr))
    {
        spng_ctx_free(ctx);
        return NULL;
    }

    /* Set the pixel pixwidth and pixheight */
    terrarium_t * t = terrarium_init(ihdr.width, ihdr.height);
    if (!t)
    {
        spng_ctx_free(ctx);
        return NULL;
    }

    /* Get the size of the image bytes */
    size_t out_size;
    if (spng_decoded_image_size(ctx, SPNG_FMT_RGB8, &out_size))
    {
        terrarium_free(t);
        spng_ctx_free(ctx);
        return NULL;
    }

    if(spng_decode_image(ctx, t->data, out_size, SPNG_FMT_RGB8, SPNG_FMT_RGB8))
    {
        terrarium_free(t);
        spng_ctx_free(ctx);
        return NULL;
    }

    spng_ctx_free(ctx);

    return t;
}