//
// Created by @redrossa on 2021-06-16.
//

#include "../include/terrarium.h"

#include "../include/spng.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

size_t terrarium_tile_flush(tile_data_t * td, va_list args)
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

static size_t dispatch_error(spng_ctx * ctx, char * msg, int mem, ...)
{
    fprintf(stderr, "%s\n", msg);
    spng_ctx_free(ctx);
    va_list args;
    va_start(args, mem);
    for (int i = 0; i < mem; i++)
        free(va_arg(args, void *));
    va_end(args);
    return 0;
}

size_t terrarium_heights(unsigned char * png_bytes, size_t size, terrarium_t * terra)
{
    /* Initialize spng context */
    spng_ctx * ctx = spng_ctx_new(0);
    if (!ctx)
        return dispatch_error(ctx, "Cannot initialize context", 0);

    /* Set context to the buffer containing the terrarium data encoded in png */
    if (spng_set_png_buffer(ctx, png_bytes, size))
        return dispatch_error(ctx, "Cannot set buffer", 0);

    /* Retrieve IHDR */
    struct spng_ihdr ihdr;
    if (spng_get_ihdr(ctx, &ihdr))
        return dispatch_error(ctx, "Cannot retrieve IHDR", 0);

    /* Set the pixel pixwidth and pixheight */
    terra->pixwidth = ihdr.width;
    terra->pixheight = ihdr.height;

    /* Get the size of the image bytes */
    size_t out_size;
    if (spng_decoded_image_size(ctx, SPNG_FMT_RGB8, &out_size))
        return dispatch_error(ctx, "Cannot retrieve image size", 0);

    /* Get the pixwidth of the image bytes */
    size_t out_width = out_size / ihdr.height;

    unsigned char * tmp = malloc(sizeof(unsigned char) * out_size);
    if (!tmp)
        return dispatch_error(ctx, "Cannot allocate memory", 0);
    if(spng_decode_image(ctx, tmp, out_size, SPNG_FMT_RGB8, SPNG_FMT_RGB8))
        return dispatch_error(ctx, "Cannot decode image", 1, (void *) tmp);

    /* Allocate terrarium rows */
    terra->rows = malloc(sizeof(unsigned char *) * ihdr.height);
    if (!terra->rows)
        return dispatch_error(ctx, "Cannot allocate memory", 1, (void *) tmp);

    for (int i = 0; i < ihdr.height; i++)
    {
        terra->rows[i] = malloc(sizeof(unsigned char) * out_width);
        if (!terra->rows[i])
        {
            for (int j = 0; j < i; j++)
                free(terra->rows[j]);
            return dispatch_error(ctx, "Cannot allocate memory", 2, (void *) tmp, (void *) terra->rows);
        }
        memcpy(terra->rows[i], tmp + i * out_width, out_width);
    }

    spng_ctx_free(ctx);

    return out_size;
}