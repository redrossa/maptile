//
// Created by @redrossa on 2021-06-20.
//

#include "../include/terrarium.h"

#include "../include/spng.h"
#include "../include/lodepng.h"

#include <vector>
#include <cstdio>
#include <algorithm>

using namespace terrarium;
using namespace maptile;

#define MAPZEN_ENDPOINT_FORMAT "https://s3.amazonaws.com/elevation-tiles-prod/terrarium/%d/%d/%d.png"

char *terrarium::tile_url_init(map *m, index_t x, index_t y)
{
    char *url = static_cast<char *>(malloc(sizeof(char) * 128));
    if (!url)
        return nullptr;
    sprintf(url, MAPZEN_ENDPOINT_FORMAT, m->get_zoom(), x, y);
    return url;
}

byte_t *terrarium::png_decode(size_t *nmemb, size_t *pixw_dst, size_t *pixh_dst, byte_t *img, size_t size)
{
    int error;

    spng_ctx *ctx = spng_ctx_new(0);
    if (!ctx)
        return nullptr;
    spng_set_png_buffer(ctx, img, size);

    struct spng_ihdr ihdr;
    error = spng_get_ihdr(ctx, &ihdr);
    if (error)
    {
        spng_ctx_free(ctx);
        return nullptr;
    }

    *pixw_dst = ihdr.width;
    *pixh_dst = ihdr.height;

    error = spng_decoded_image_size(ctx, SPNG_FMT_RGB8, nmemb);
    if (error)
    {
        spng_ctx_free(ctx);
        return nullptr;
    }

    auto *decoded = static_cast<byte_t *>(malloc(sizeof(byte_t) * *nmemb));
    if (!decoded)
    {
        spng_ctx_free(ctx);
        return nullptr;
    }
    error = spng_decode_image(ctx, decoded, *nmemb, SPNG_FMT_RGB8, 0);
    if (error)
    {
        free(decoded);
        spng_ctx_free(ctx);
        return nullptr;
    }

    spng_ctx_free(ctx);

    return decoded;
}

#define FNAME_Z_LENGTH 2
#define FNAME_Y_LENGTH 5
#define FNAME_X_LENGTH 5

int terrarium::direct_tile_fflush(index_t itc, map *m, tile_transfer_t *tile_transf, va_list args)
{
    char *dirname = va_arg(args, char *);

    char fname[256];
    sprintf(fname, "%s/%0*d-%0*d-%0*d.terrarium.png",
            dirname,
            FNAME_Z_LENGTH, tile_transf->z,
            FNAME_X_LENGTH, tile_transf->x,
            FNAME_Y_LENGTH, tile_transf->y);

    FILE *fp = fopen(fname, "wb");
    if (!fp)
        return 0;
    fwrite(tile_transf->data, sizeof(*tile_transf->data), tile_transf->size, fp);

    fclose(fp);
    return 1;
}

int terrarium::map_fflush(index_t itc, map *m, tile_transfer_t *tile_transf, va_list args)
{
    size_t decoded_size, pixw, pixh;
    byte_t *tmp = png_decode(&decoded_size, &pixw, &pixh, tile_transf->data, tile_transf->size);
    if (!tmp)
        return 0;

    size_t pix_size = decoded_size / pixw / pixh;
    tile *t = new tile(m->get_zoom(), tile_transf->x, tile_transf->y, pixw, pixh, pix_size);
    t->set_pix_bytes(tmp, decoded_size);
    m->set_tile(t);

    if (itc == m->get_tile_count())
    {
        size_t size = m->expected_size();
        auto *merged = static_cast<byte_t *>(malloc(sizeof(byte_t) * size));
        if (!merged)
            return 0;
        m->merge_tile_data_to_buf(merged, size);

        char * dirname = va_arg(args, char *);
        char fname[256];
        sprintf(fname, "%s/%0*d-%0*d-%0*d-%0*d-%0*d.terrarium.png",
                dirname,
                FNAME_Z_LENGTH, m->get_zoom(),
                FNAME_X_LENGTH, m->get_xmin(),
                FNAME_Y_LENGTH, m->get_ymin(),
                FNAME_X_LENGTH, m->get_xmax(),
                FNAME_Y_LENGTH, m->get_xmin());

        lodepng_encode24_file(fname, merged,m->get_xshape() * t->get_pixw(),m->get_yshape() * t->get_pixh());

        free(merged);
    }

    return 1;
}
