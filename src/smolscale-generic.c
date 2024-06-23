/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/* Copyright © 2019-2023 Hans Petter Jansson. See COPYING for details. */

#include <stdlib.h> /* malloc, free, alloca */
#include <string.h> /* memset */
#include <limits.h>
#include "smolscale-private.h"

/* ---------------------- *
 * Context initialization *
 * ---------------------- */

static void
precalc_bilinear_array (uint16_t *array,
                        uint32_t dim_in,
                        uint32_t dim_out,
                        unsigned int make_absolute_offsets)
{
    uint64_t ofs_stepF, fracF, frac_stepF;
    uint16_t *pu16 = array;
    uint16_t last_ofs = 0;

    if (dim_in > dim_out)
    {
        /* Minification */
        frac_stepF = ofs_stepF = (dim_in * SMOL_BILIN_MULTIPLIER) / dim_out;
        fracF = (frac_stepF - SMOL_BILIN_MULTIPLIER) / 2;
    }
    else
    {
        /* Magnification */
        frac_stepF = ofs_stepF = ((dim_in - 1) * SMOL_BILIN_MULTIPLIER)
            / (dim_out > 1 ? (dim_out - 1) : 1);
        fracF = 0;
    }

    do
    {
        uint16_t ofs = (uint16_t)(fracF / SMOL_BILIN_MULTIPLIER);

        /* We sample ofs and its neighbor -- prevent out of bounds access
         * for the latter. */
        if (ofs >= dim_in - 1)
            break;

        *(pu16++) = make_absolute_offsets ? ofs : ofs - last_ofs;
        *(pu16++) = SMOL_SMALL_MUL - ((fracF / (SMOL_BILIN_MULTIPLIER / SMOL_SMALL_MUL))
                                      % SMOL_SMALL_MUL);
        fracF += frac_stepF;

        last_ofs = ofs;
    }
    while (--dim_out);

    /* Instead of going out of bounds, sample the final pair of pixels with a 100%
     * bias towards the last pixel */
    while (dim_out)
    {
        *(pu16++) = make_absolute_offsets ? (uint16_t)(dim_in - 2) : (uint16_t)((dim_in - 2) - last_ofs);
        *(pu16++) = 0;
        dim_out--;

        last_ofs = (uint16_t)(dim_in - 2);
    }
}

static void
precalc_boxes_array (uint16_t *array,
                     uint32_t *span_mul,
                     uint32_t dim_in,
                     uint32_t dim_out,
                     unsigned int make_absolute_offsets)
{
    uint64_t fracF, frac_stepF;
    uint16_t *pu16 = array;
    uint16_t ofs, next_ofs;
    uint64_t f;
    uint64_t stride;
    uint64_t a, b;

    frac_stepF = ((uint64_t) dim_in * SMOL_BIG_MUL) / (uint64_t) dim_out;
    fracF = 0;
    ofs = 0;

    stride = frac_stepF / (uint64_t) SMOL_BIG_MUL;
    f = (frac_stepF / SMOL_SMALL_MUL) % SMOL_SMALL_MUL;

    a = (SMOL_BOXES_MULTIPLIER * 255);
    b = ((stride * 255) + ((f * 255) / 256));
    *span_mul = (uint32_t)((a + (b / 2)) / b);

    do
    {
        fracF += frac_stepF;
        next_ofs = (uint16_t)( (uint64_t) fracF / ((uint64_t) SMOL_BIG_MUL) );

        /* Prevent out of bounds access */
        if (ofs >= dim_in - 1)
        {
            ofs = (uint16_t)(dim_in - 1);
            break;
        }

        if (next_ofs > dim_in - 1)
        {
            next_ofs = (uint16_t)(dim_in - 1);
            if (next_ofs <= ofs)
                break;
        }

        stride = next_ofs - ofs - 1;
        f = (fracF / SMOL_SMALL_MUL) % SMOL_SMALL_MUL;

        /* Fraction is the other way around, since left pixel of each span
         * comes first, and it's on the right side of the fractional sample. */
        *(pu16++) = (uint16_t)(make_absolute_offsets ? ofs : stride);
        *(pu16++) = (uint16_t)f;

        ofs = next_ofs;
    }
    while (--dim_out);

    /* Instead of going out of bounds, sample the final pair of pixels with a 100%
     * bias towards the last pixel */
    while (dim_out)
    {
        *(pu16++) = make_absolute_offsets ? ofs : 0;
        *(pu16++) = 0;
        dim_out--;
    }

    *(pu16++) = make_absolute_offsets ? ofs : 0;
    *(pu16++) = 0;
}

static void
init_horizontal (SmolScaleCtx *scale_ctx)
{
    if (scale_ctx->filter_h == SMOL_FILTER_ONE
        || scale_ctx->filter_h == SMOL_FILTER_COPY)
    {
    }
    else if (scale_ctx->filter_h == SMOL_FILTER_BOX)
    {
        precalc_boxes_array (scale_ctx->precalc_x, &scale_ctx->span_mul_x,
                             scale_ctx->width_in, scale_ctx->width_out,
                             FALSE);
    }
    else /* SMOL_FILTER_BILINEAR_?H */
    {
        precalc_bilinear_array (scale_ctx->precalc_x,
                                scale_ctx->width_in,
                                scale_ctx->width_bilin_out,
                                FALSE);
    }
}

static void
init_vertical (SmolScaleCtx *scale_ctx)
{
    if (scale_ctx->filter_v == SMOL_FILTER_ONE
        || scale_ctx->filter_v == SMOL_FILTER_COPY)
    {
    }
    else if (scale_ctx->filter_v == SMOL_FILTER_BOX)
    {
        precalc_boxes_array (scale_ctx->precalc_y, &scale_ctx->span_mul_y,
                             scale_ctx->height_in, scale_ctx->height_out,
                             TRUE);
    }
    else /* SMOL_FILTER_BILINEAR_?H */
    {
        precalc_bilinear_array (scale_ctx->precalc_y,
                                scale_ctx->height_in,
                                scale_ctx->height_bilin_out,
                                TRUE);
    }
}

/* ---------------------- *
 * sRGB/linear conversion *
 * ---------------------- */

static void
from_srgb_pixel_xxxa_128bpp (uint64_t * SMOL_RESTRICT pixel_inout)
{
    uint64_t part;

    part = *pixel_inout;
    *(pixel_inout++) =
        ((uint64_t) _smol_from_srgb_lut [part >> 32] << 32)
        | _smol_from_srgb_lut [part & 0xff];

    part = *pixel_inout;
    *pixel_inout =
        ((uint64_t) _smol_from_srgb_lut [part >> 32] << 32)
        | (part & 0xffffffff);
}

static void
to_srgb_pixel_xxxa_128bpp (const uint64_t *pixel_in, uint64_t *pixel_out)
{
    pixel_out [0] =
        (((uint64_t) _smol_to_srgb_lut [pixel_in [0] >> 32]) << 32)
        | _smol_to_srgb_lut [pixel_in [0] & 0xffff];

    pixel_out [1] =
        (((uint64_t) _smol_to_srgb_lut [pixel_in [1] >> 32]) << 32)
        | (pixel_in [1] & 0xffffffff);
}

/* ----------------- *
 * Premultiplication *
 * ----------------- */

static SMOL_INLINE void
premul_u_to_p8_128bpp (uint64_t * SMOL_RESTRICT inout,
                       uint8_t alpha)
{
    inout [0] = (((inout [0] + 0x0000000100000001) * ((uint16_t) alpha + 1) - 0x0000000100000001)
                 >> 8) & 0x000000ff000000ff;
    inout [1] = (((inout [1] + 0x0000000100000001) * ((uint16_t) alpha + 1) - 0x0000000100000001)
                 >> 8) & 0x000000ff000000ff;
}

static SMOL_INLINE void
unpremul_p8_to_u_128bpp (const uint64_t *in,
                         uint64_t *out,
                         uint8_t alpha)
{
    out [0] = ((in [0] * _smol_inv_div_p8_lut [alpha])
               >> INVERTED_DIV_SHIFT_P8) & 0x000000ff000000ff;
    out [1] = ((in [1] * _smol_inv_div_p8_lut [alpha])
               >> INVERTED_DIV_SHIFT_P8) & 0x000000ff000000ff;
}

static SMOL_INLINE uint64_t
premul_u_to_p8_64bpp (const uint64_t in,
                      uint8_t alpha)
{
    return (((in + 0x0001000100010001) * ((uint16_t) alpha + 1) - 0x0001000100010001)
            >> 8) & 0x00ff00ff00ff00ff;
}

static SMOL_INLINE uint64_t
unpremul_p8_to_u_64bpp (const uint64_t in,
                        uint8_t alpha)
{
    uint64_t in_128bpp [2];
    uint64_t out_128bpp [2];

    in_128bpp [0] = (in & 0x000000ff000000ff);
    in_128bpp [1] = (in & 0x00ff000000ff0000) >> 16;

    unpremul_p8_to_u_128bpp (in_128bpp, out_128bpp, alpha);

    return out_128bpp [0] | (out_128bpp [1] << 16);
}

static SMOL_INLINE void
premul_ul_to_p8l_128bpp (uint64_t * SMOL_RESTRICT inout,
                         uint8_t alpha)
{
    inout [0] = (((inout [0] + 0x0000000100000001) * (uint64_t)(((uint16_t) alpha << 3) + 1) - 0x0000000100000001)
                 >> 11) & 0x000007ff000007ff;
    inout [1] = (((inout [1] + 0x0000000100000001) * (uint64_t)(((uint16_t) alpha << 3) + 1) - 0x0000000100000001)
                 >> 11) & 0x000007ff000007ff;
}

static SMOL_INLINE void
unpremul_p8l_to_ul_128bpp (const uint64_t *in,
                           uint64_t *out,
                           uint8_t alpha)
{
    out [0] = ((in [0] * _smol_inv_div_p8l_lut [alpha])
               >> INVERTED_DIV_SHIFT_P8L) & 0x000007ff000007ff;
    out [1] = ((in [1] * _smol_inv_div_p8l_lut [alpha])
               >> INVERTED_DIV_SHIFT_P8L) & 0x000007ff000007ff;
}

// static SMOL_INLINE void
// premul_u_to_p16_128bpp (uint64_t *inout,
//                         uint8_t alpha)
// {
//     inout [0] = inout [0] * alpha;
//     inout [1] = inout [1] * alpha;
// }

static SMOL_INLINE void
unpremul_p16_to_u_128bpp (const uint64_t * SMOL_RESTRICT in,
                          uint64_t * SMOL_RESTRICT out,
                          uint8_t alpha)
{
    out [0] = ((in [0] * _smol_inv_div_p16_lut [alpha])
               >> INVERTED_DIV_SHIFT_P16) & 0x000000ff000000ffULL;
    out [1] = ((in [1] * _smol_inv_div_p16_lut [alpha])
               >> INVERTED_DIV_SHIFT_P16) & 0x000000ff000000ffULL;
}

// static SMOL_INLINE void
// premul_ul_to_p16l_128bpp (uint64_t *inout,
//                           uint8_t alpha)
// {
//     inout [0] = inout [0] * alpha;
//     inout [1] = inout [1] * alpha;
// }

static SMOL_INLINE void
unpremul_p16l_to_ul_128bpp (const uint64_t * SMOL_RESTRICT in,
                            uint64_t * SMOL_RESTRICT out,
                            uint8_t alpha)
{
    out [0] = ((in [0] * _smol_inv_div_p16l_lut [alpha])
               >> INVERTED_DIV_SHIFT_P16L) & 0x000007ff000007ffULL;
    out [1] = ((in [1] * _smol_inv_div_p16l_lut [alpha])
               >> INVERTED_DIV_SHIFT_P16L) & 0x000007ff000007ffULL;
}

/* --------- *
 * Repacking *
 * --------- */

/* It's nice to be able to shift by a negative amount */
#define SHIFT_S(in, s) ((s >= 0) ? (in) << (s) : (in) >> -(s))

/* This is kind of bulky (~13 x86 insns), but it's about the same as using
 * unions, and we don't have to worry about endianness. */
#define PACK_FROM_1234_64BPP(in, a, b, c, d) \
    ((SHIFT_S ((in), ((a) - 1) * 16 + 8 - 32) & 0xff000000) \
     | (SHIFT_S ((in), ((b) - 1) * 16 + 8 - 40) & 0x00ff0000) \
     | (SHIFT_S ((in), ((c) - 1) * 16 + 8 - 48) & 0x0000ff00) \
     | (SHIFT_S ((in), ((d) - 1) * 16 + 8 - 56) & 0x000000ff))

#define PACK_FROM_1234_128BPP(in, a, b, c, d) \
    ((SHIFT_S ((in [((a) - 1) >> 1]), (((a) - 1) & 1) * 32 + 24 - 32) & 0xff000000) \
     | (SHIFT_S ((in [((b) - 1) >> 1]), (((b) - 1) & 1) * 32 + 24 - 40) & 0x00ff0000) \
     | (SHIFT_S ((in [((c) - 1) >> 1]), (((c) - 1) & 1) * 32 + 24 - 48) & 0x0000ff00) \
     | (SHIFT_S ((in [((d) - 1) >> 1]), (((d) - 1) & 1) * 32 + 24 - 56) & 0x000000ff))

#define SWAP_2_AND_3(n) ((n) == 2 ? 3 : (n) == 3 ? 2 : n)

#define PACK_FROM_1324_64BPP(in, a, b, c, d) \
    ((SHIFT_S ((in), (SWAP_2_AND_3 (a) - 1) * 16 + 8 - 32) & 0xff000000) \
     | (SHIFT_S ((in), (SWAP_2_AND_3 (b) - 1) * 16 + 8 - 40) & 0x00ff0000) \
     | (SHIFT_S ((in), (SWAP_2_AND_3 (c) - 1) * 16 + 8 - 48) & 0x0000ff00) \
     | (SHIFT_S ((in), (SWAP_2_AND_3 (d) - 1) * 16 + 8 - 56) & 0x000000ff))

/* ---------------------- *
 * Repacking: 24/32 -> 64 *
 * ---------------------- */

static SMOL_INLINE uint64_t
unpack_pixel_123_p8_to_132a_p8_64bpp (const uint8_t *p)
{
    return ((uint64_t) p [0] << 48) | ((uint32_t) p [1] << 16)
        | ((uint64_t) p [2] << 32) | 0xff;
}

SMOL_REPACK_ROW_DEF (123,  24,  8, PREMUL8, COMPRESSED,
                     1324, 64, 64, PREMUL8, COMPRESSED) {
    while (row_out != row_out_max)
    {
        *(row_out++) = unpack_pixel_123_p8_to_132a_p8_64bpp (row_in);
        row_in += 3;
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE uint64_t
unpack_pixel_1234_p8_to_1324_p8_64bpp (uint32_t p)
{
    return (((uint64_t) p & 0xff00ff00) << 24) | (p & 0x00ff00ff);
}

SMOL_REPACK_ROW_DEF (1234, 32, 32, PREMUL8, COMPRESSED,
                     1324, 64, 64, PREMUL8, COMPRESSED) {
    while (row_out != row_out_max)
    {
        *(row_out++) = unpack_pixel_1234_p8_to_1324_p8_64bpp (*(row_in++));
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE uint64_t
unpack_pixel_1234_p8_to_3241_p8_64bpp (uint32_t p)
{
    return (((uint64_t) p & 0x0000ff00) << 40)
        | (((uint64_t) p & 0x00ff00ff) << 16) | (p >> 24);
}

SMOL_REPACK_ROW_DEF (1234, 32, 32, PREMUL8, COMPRESSED,
                     3241, 64, 64, PREMUL8, COMPRESSED) {
    while (row_out != row_out_max)
    {
        *(row_out++) = unpack_pixel_1234_p8_to_3241_p8_64bpp (*(row_in++));
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE uint64_t
unpack_pixel_1234_p8_to_2431_p8_64bpp (uint32_t p)
{
    uint64_t p64 = p;

    return ((p64 & 0x00ff00ff) << 32) | ((p64 & 0x0000ff00) << 8)
        | ((p64 & 0xff000000) >> 24);
}

SMOL_REPACK_ROW_DEF (1234, 32, 32, PREMUL8, COMPRESSED,
                     2431, 64, 64, PREMUL8, COMPRESSED) {
    while (row_out != row_out_max)
    {
        *(row_out++) = unpack_pixel_1234_p8_to_2431_p8_64bpp (*(row_in++));
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE uint64_t
unpack_pixel_a234_u_to_324a_p8_64bpp (uint32_t p)
{
    uint64_t p64 = (((uint64_t) p & 0x0000ff00) << 40) | (((uint64_t) p & 0x00ff00ff) << 16);
    uint8_t alpha = p >> 24;

    return (premul_u_to_p8_64bpp (p64, alpha) & 0xffffffffffffff00ULL) | alpha;
}

SMOL_REPACK_ROW_DEF (1234, 32, 32, UNASSOCIATED, COMPRESSED,
                     3241, 64, 64, PREMUL8,      COMPRESSED) {
    while (row_out != row_out_max)
    {
        *(row_out++) = unpack_pixel_a234_u_to_324a_p8_64bpp (*(row_in++));
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE uint64_t
unpack_pixel_1234_u_to_2431_p8_64bpp (uint32_t p)
{
    uint64_t p64 = (((uint64_t) p & 0x00ff00ff) << 32) | (((uint64_t) p & 0x0000ff00) << 8);
    uint8_t alpha = p >> 24;

    return (premul_u_to_p8_64bpp (p64, alpha) & 0xffffffffffffff00ULL) | alpha;
}

SMOL_REPACK_ROW_DEF (1234, 32, 32, UNASSOCIATED, COMPRESSED,
                     2431, 64, 64, PREMUL8,      COMPRESSED) {
    while (row_out != row_out_max)
    {
        *(row_out++) = unpack_pixel_1234_u_to_2431_p8_64bpp (*(row_in++));
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE uint64_t
unpack_pixel_123a_u_to_132a_p8_64bpp (uint32_t p)
{
    uint64_t p64 = (((uint64_t) p & 0xff00ff00) << 24) | (p & 0x00ff0000);
    uint8_t alpha = p & 0xff;

    return (premul_u_to_p8_64bpp (p64, alpha) & 0xffffffffffffff00ULL) | alpha;
}

SMOL_REPACK_ROW_DEF (1234, 32, 32, UNASSOCIATED, COMPRESSED,
                     1324, 64, 64, PREMUL8,      COMPRESSED) {
    while (row_out != row_out_max)
    {
        *(row_out++) = unpack_pixel_123a_u_to_132a_p8_64bpp (*(row_in++));
    }
} SMOL_REPACK_ROW_DEF_END

/* ----------------------- *
 * Repacking: 24/32 -> 128 *
 * ----------------------- */

static SMOL_INLINE void
unpack_pixel_123_p8_to_123a_p8_128bpp (const uint8_t *in,
                                       uint64_t *out)
{
    out [0] = ((uint64_t) in [0] << 32) | in [1];
    out [1] = ((uint64_t) in [2] << 32) | 0xff;
}

SMOL_REPACK_ROW_DEF (123,   24,  8, PREMUL8, COMPRESSED,
                     1234, 128, 64, PREMUL8, COMPRESSED) {
    while (row_out != row_out_max)
    {
        unpack_pixel_123_p8_to_123a_p8_128bpp (row_in, row_out);
        row_in += 3;
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (123,   24,  8, PREMUL8, COMPRESSED,
                     1234, 128, 64, PREMUL8, LINEAR) {
    while (row_out != row_out_max)
    {
        uint8_t alpha;
        unpack_pixel_123_p8_to_123a_p8_128bpp (row_in, row_out);
        alpha = (uint8_t)row_out [1];
        unpremul_p8_to_u_128bpp (row_out, row_out, alpha);
        from_srgb_pixel_xxxa_128bpp (row_out);
        premul_ul_to_p8l_128bpp (row_out, alpha);
        row_out [1] = (row_out [1] & 0xffffffff00000000ULL) | alpha;
        row_in += 3;
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE void
unpack_pixel_123a_p8_to_123a_p8_128bpp (uint32_t p,
                                        uint64_t *out)
{
    uint64_t p64 = p;
    out [0] = ((p64 & 0xff000000) << 8) | ((p64 & 0x00ff0000) >> 16);
    out [1] = ((p64 & 0x0000ff00) << 24) | (p64 & 0x000000ff);
}

SMOL_REPACK_ROW_DEF (1234,  32, 32, PREMUL8, COMPRESSED,
                     1234, 128, 64, PREMUL8, COMPRESSED) {
    while (row_out != row_out_max)
    {
        unpack_pixel_123a_p8_to_123a_p8_128bpp (*(row_in++), row_out);
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234,  32, 32, PREMUL8, COMPRESSED,
                     1234, 128, 64, PREMUL8, LINEAR) {
    while (row_out != row_out_max)
    {
        uint8_t alpha;
        unpack_pixel_123a_p8_to_123a_p8_128bpp (*(row_in++), row_out);
        alpha = (uint8_t)row_out [1];
        unpremul_p8_to_u_128bpp (row_out, row_out, alpha);
        from_srgb_pixel_xxxa_128bpp (row_out);
        premul_ul_to_p8l_128bpp (row_out, alpha);
        row_out [1] = (row_out [1] & 0xffffffff00000000ULL) | alpha;
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE void
unpack_pixel_a234_p8_to_234a_p8_128bpp (uint32_t p,
                                        uint64_t *out)
{
    uint64_t p64 = p;
    out [0] = ((p64 & 0x00ff0000) << 16) | ((p64 & 0x0000ff00) >> 8);
    out [1] = ((p64 & 0x000000ff) << 32) | ((p64 & 0xff000000) >> 24);
}

SMOL_REPACK_ROW_DEF (1234,  32, 32, PREMUL8, COMPRESSED,
                     2341, 128, 64, PREMUL8, COMPRESSED) {
    while (row_out != row_out_max)
    {
        unpack_pixel_a234_p8_to_234a_p8_128bpp (*(row_in++), row_out);
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234,  32, 32, PREMUL8, COMPRESSED,
                     2341, 128, 64, PREMUL8, LINEAR) {
    while (row_out != row_out_max)
    {
        uint8_t alpha;
        unpack_pixel_a234_p8_to_234a_p8_128bpp (*(row_in++), row_out);
        alpha = (uint8_t)row_out [1];
        unpremul_p8_to_u_128bpp (row_out, row_out, alpha);
        from_srgb_pixel_xxxa_128bpp (row_out);
        premul_ul_to_p8l_128bpp (row_out, alpha);
        row_out [1] = (row_out [1] & 0xffffffff00000000ULL) | alpha;
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE void
unpack_pixel_a234_u_to_234a_p8_128bpp (uint32_t p,
                                       uint64_t *out)
{
    uint64_t p64 = (((uint64_t) p & 0x00ff00ff) << 32) | (((uint64_t) p & 0x0000ff00) << 8);
    uint8_t alpha = p >> 24;

    p64 = (premul_u_to_p8_64bpp (p64, alpha) & 0xffffffffffffff00) | alpha;
    out [0] = (p64 >> 16) & 0x000000ff000000ff;
    out [1] = p64 & 0x000000ff000000ff;
}

SMOL_REPACK_ROW_DEF (1234,  32, 32, UNASSOCIATED, COMPRESSED,
                     2341, 128, 64, PREMUL8,      COMPRESSED) {
    while (row_out != row_out_max)
    {
        unpack_pixel_a234_u_to_234a_p8_128bpp (*(row_in++), row_out);
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE void
unpack_pixel_a234_u_to_234a_pl_128bpp (uint32_t p,
                                       uint64_t *out)
{
    uint64_t p64 = p;
    uint8_t alpha = p >> 24;

    out [0] = ((p64 & 0x00ff0000) << 16) | ((p64 & 0x0000ff00) >> 8);
    out [1] = ((p64 & 0x000000ff) << 32);

    from_srgb_pixel_xxxa_128bpp (out);
    premul_ul_to_p8l_128bpp (out, alpha);

    out [1] = (out [1] & 0xffffffffffffff00ULL) | alpha;
}

SMOL_REPACK_ROW_DEF (1234,  32, 32, UNASSOCIATED, COMPRESSED,
                     2341, 128, 64, PREMUL8,      LINEAR) {
    while (row_out != row_out_max)
    {
        unpack_pixel_a234_u_to_234a_pl_128bpp (*(row_in++), row_out);
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE void
unpack_pixel_a234_u_to_234a_p16_128bpp (uint32_t p,
                                        uint64_t *out)
{
    uint64_t p64 = p;
    uint8_t alpha = p >> 24;

    out [0] = (((((p64 & 0x00ff0000) << 16) | ((p64 & 0x0000ff00) >> 8)) * alpha));
    out [1] = (((((p64 & 0x000000ff) << 32) * alpha))) | (uint64_t)((uint16_t) alpha << 8) | 0x80;
}

SMOL_REPACK_ROW_DEF (1234,  32, 32, UNASSOCIATED, COMPRESSED,
                     2341, 128, 64, PREMUL16,     COMPRESSED) {
    while (row_out != row_out_max)
    {
        unpack_pixel_a234_u_to_234a_p16_128bpp (*(row_in++), row_out);
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE void
unpack_pixel_a234_u_to_234a_p16l_128bpp (uint32_t p,
                                         uint64_t *out)
{
    uint64_t p64 = p;
    uint8_t alpha = p >> 24;

    out [0] = ((p64 & 0x00ff0000) << 16) | ((p64 & 0x0000ff00) >> 8);
    out [1] = ((p64 & 0x000000ff) << 32);

    from_srgb_pixel_xxxa_128bpp (out);
    out [0] *= alpha;
    out [1] *= alpha;

    out [1] = (out [1] & 0xffffffff00000000ULL) | (uint64_t)(alpha << 8) | 0x80;
}

SMOL_REPACK_ROW_DEF (1234,  32, 32, UNASSOCIATED, COMPRESSED,
                     2341, 128, 64, PREMUL16,     LINEAR) {
    while (row_out != row_out_max)
    {
        unpack_pixel_a234_u_to_234a_p16l_128bpp (*(row_in++), row_out);
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE void
unpack_pixel_123a_u_to_123a_p8_128bpp (uint32_t p,
                                       uint64_t *out)
{
    uint64_t p64 = (((uint64_t) p & 0xff00ff00) << 24) | (p & 0x00ff0000);
    uint8_t alpha = (uint8_t)p;

    p64 = (premul_u_to_p8_64bpp (p64, alpha) & 0xffffffffffffff00ULL) | alpha;
    out [0] = (p64 >> 16) & 0x000000ff000000ff;
    out [1] = p64 & 0x000000ff000000ff;
}

SMOL_REPACK_ROW_DEF (1234,  32, 32, UNASSOCIATED, COMPRESSED,
                     1234, 128, 64, PREMUL8,      COMPRESSED) {
    while (row_out != row_out_max)
    {
        unpack_pixel_123a_u_to_123a_p8_128bpp (*(row_in++), row_out);
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE void
unpack_pixel_123a_u_to_123a_pl_128bpp (uint32_t p,
                                       uint64_t *out)
{
    uint64_t p64 = p;
    uint8_t alpha = (uint8_t)p;

    out [0] = ((p64 & 0xff000000) << 8) | ((p64 & 0x00ff0000) >> 16);
    out [1] = ((p64 & 0x0000ff00) << 24);

    from_srgb_pixel_xxxa_128bpp (out);
    premul_ul_to_p8l_128bpp (out, alpha);

    out [1] = (out [1] & 0xffffffffffffff00) | alpha;
}

SMOL_REPACK_ROW_DEF (1234,  32, 32, UNASSOCIATED, COMPRESSED,
                     1234, 128, 64, PREMUL8,      LINEAR) {
    while (row_out != row_out_max)
    {
        unpack_pixel_123a_u_to_123a_pl_128bpp (*(row_in++), row_out);
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE void
unpack_pixel_123a_u_to_123a_p16_128bpp (uint32_t p,
                                        uint64_t *out)
{
    uint64_t p64 = p;
    uint8_t alpha = (uint8_t)p;

    out [0] = (((((p64 & 0xff000000) << 8) | ((p64 & 0x00ff0000) >> 16)) * alpha));
    out [1] = (((((p64 & 0x0000ff00) << 24) * alpha))) | (uint64_t)((uint16_t) alpha << 8) | 0x80;
}

SMOL_REPACK_ROW_DEF (1234,  32, 32, UNASSOCIATED, COMPRESSED,
                     1234, 128, 64, PREMUL16,     COMPRESSED) {
    while (row_out != row_out_max)
    {
        unpack_pixel_123a_u_to_123a_p16_128bpp (*(row_in++), row_out);
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

static SMOL_INLINE void
unpack_pixel_123a_u_to_123a_p16l_128bpp (uint32_t p,
                                         uint64_t *out)
{
    uint64_t p64 = p;
    uint8_t alpha = (uint8_t)p;

    out [0] = ((p64 & 0xff000000) << 8) | ((p64 & 0x00ff0000) >> 16);
    out [1] = ((p64 & 0x0000ff00) << 24);

    from_srgb_pixel_xxxa_128bpp (out);
    out [0] *= alpha;
    out [1] *= alpha;

    out [1] = (out [1] & 0xffffffffffffff00ULL) | (uint64_t)((uint16_t) alpha << 8) | 0x80;
}

SMOL_REPACK_ROW_DEF (1234,  32, 32, UNASSOCIATED, COMPRESSED,
                     1234, 128, 64, PREMUL16,     LINEAR) {
    while (row_out != row_out_max)
    {
        unpack_pixel_123a_u_to_123a_p16l_128bpp (*(row_in++), row_out);
        row_out += 2;
    }
} SMOL_REPACK_ROW_DEF_END

/* ---------------------- *
 * Repacking: 64 -> 24/32 *
 * ---------------------- */

static SMOL_INLINE uint32_t
pack_pixel_1234_p8_to_1324_p8_64bpp (uint64_t in)
{
    return (uint32_t)(in | (in >> 24));
}

SMOL_REPACK_ROW_DEF (1234, 64, 64, PREMUL8,       COMPRESSED,
                     132,  24,  8, PREMUL8,       COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint32_t p = pack_pixel_1234_p8_to_1324_p8_64bpp (*(row_in++));
        *(row_out++) = (uint8_t)(p >> 24);
        *(row_out++) = (uint8_t)(p >> 16);
        *(row_out++) = (uint8_t)(p >> 8);
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 64, 64, PREMUL8,       COMPRESSED,
                     132,  24,  8, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint8_t alpha = (uint8_t)*row_in;
        uint64_t t = (unpremul_p8_to_u_64bpp (*row_in, alpha) & 0xffffffffffffff00ULL) | alpha;
        uint32_t p = pack_pixel_1234_p8_to_1324_p8_64bpp (t);
        *(row_out++) = (uint8_t)(p >> 24);
        *(row_out++) = (uint8_t)(p >> 16);
        *(row_out++) = (uint8_t)(p >> 8);
        row_in++;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 64, 64, PREMUL8,       COMPRESSED,
                     231,  24,  8, PREMUL8,       COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint32_t p = pack_pixel_1234_p8_to_1324_p8_64bpp (*(row_in++));
        *(row_out++) = (uint8_t)(p >> 8);
        *(row_out++) = (uint8_t)(p >> 16);
        *(row_out++) = (uint8_t)(p >> 24);
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 64, 64, PREMUL8,       COMPRESSED,
                     231,  24,  8, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint8_t alpha = (uint8_t)*row_in;
        uint64_t t = (unpremul_p8_to_u_64bpp (*row_in, alpha) & 0xffffffffffffff00ULL) | alpha;
        uint32_t p = pack_pixel_1234_p8_to_1324_p8_64bpp (t);
        *(row_out++) = (uint8_t)(p >> 8);
        *(row_out++) = (uint8_t)(p >> 16);
        *(row_out++) = (uint8_t)(p >> 24);
        row_in++;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 64, 64, PREMUL8,       COMPRESSED,
                     324,  24,  8, PREMUL8,       COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint32_t p = pack_pixel_1234_p8_to_1324_p8_64bpp (*(row_in++));
        *(row_out++) = (uint8_t)(p >> 16);
        *(row_out++) = (uint8_t)(p >>  8);
        *(row_out++) = (uint8_t)(p      );
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 64, 64, PREMUL8,       COMPRESSED,
                     324,  24,  8, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint8_t alpha = (uint8_t)(*row_in >> 24);
        uint64_t t = (unpremul_p8_to_u_64bpp (*row_in, alpha) & 0xffffffffffffff00ULL) | alpha;
        uint32_t p = pack_pixel_1234_p8_to_1324_p8_64bpp (t);
        *(row_out++) = (uint8_t)(p >> 16);
        *(row_out++) = (uint8_t)(p >>  8);
        *(row_out++) = (uint8_t)(p);
        row_in++;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 64, 64, PREMUL8,       COMPRESSED,
                     423,  24,  8, PREMUL8,       COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint32_t p = pack_pixel_1234_p8_to_1324_p8_64bpp (*(row_in++));
        *(row_out++) = (uint8_t)(p);
        *(row_out++) = (uint8_t)(p >>  8);
        *(row_out++) = (uint8_t)(p >> 16);
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 64, 64, PREMUL8,       COMPRESSED,
                     423,  24,  8, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint8_t alpha = (uint8_t)(*row_in >> 24);
        uint64_t t = (unpremul_p8_to_u_64bpp (*row_in, alpha) & 0xffffffffffffff00ULL) | alpha;
        uint32_t p = pack_pixel_1234_p8_to_1324_p8_64bpp (t);
        *(row_out++) = (uint8_t)(p);
        *(row_out++) = (uint8_t)(p >> 8);
        *(row_out++) = (uint8_t)(p >> 16);
        row_in++;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 64, 64, PREMUL8,       COMPRESSED,
                     1324, 32, 32, PREMUL8,       COMPRESSED) {
    while (row_out != row_out_max)
    {
        *(row_out++) = pack_pixel_1234_p8_to_1324_p8_64bpp (*(row_in++));
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 64, 64, PREMUL8,       COMPRESSED,
                     1324, 32, 32, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint8_t alpha = (uint8_t)(*row_in);
        uint64_t t = (unpremul_p8_to_u_64bpp (*row_in, alpha) & 0xffffffffffffff00ULL) | alpha;
        *(row_out++) = pack_pixel_1234_p8_to_1324_p8_64bpp (t);
        row_in++;
    }
} SMOL_REPACK_ROW_DEF_END

#define DEF_REPACK_FROM_1234_64BPP_TO_32BPP(a, b, c, d) \
    SMOL_REPACK_ROW_DEF (1234,       64, 64, PREMUL8,       COMPRESSED, \
                         a##b##c##d, 32, 32, PREMUL8,       COMPRESSED) { \
        while (row_out != row_out_max) \
        { \
            *(row_out++) = PACK_FROM_1234_64BPP (*row_in, a, b, c, d); \
            row_in++; \
        } \
    } SMOL_REPACK_ROW_DEF_END \
    SMOL_REPACK_ROW_DEF (1234,       64, 64, PREMUL8,       COMPRESSED, \
                         a##b##c##d, 32, 32, UNASSOCIATED,  COMPRESSED) { \
        while (row_out != row_out_max) \
        { \
            uint8_t alpha = (uint8_t)(*row_in); \
            uint64_t t = (unpremul_p8_to_u_64bpp (*row_in, alpha) & 0xffffffffffffff00ULL) | alpha; \
            *(row_out++) = PACK_FROM_1234_64BPP (t, a, b, c, d); \
            row_in++; \
        } \
    } SMOL_REPACK_ROW_DEF_END

DEF_REPACK_FROM_1234_64BPP_TO_32BPP (1, 4, 2, 3)
DEF_REPACK_FROM_1234_64BPP_TO_32BPP (2, 3, 1, 4)
DEF_REPACK_FROM_1234_64BPP_TO_32BPP (4, 1, 3, 2)
DEF_REPACK_FROM_1234_64BPP_TO_32BPP (4, 2, 3, 1)

/* ----------------------- *
 * Repacking: 128 -> 24/32 *
 * ----------------------- */

SMOL_REPACK_ROW_DEF (1234, 128, 64, PREMUL8,       COMPRESSED,
                     123,   24,  8, PREMUL8,       COMPRESSED) {
    while (row_out != row_out_max)
    {
        *(row_out++) = (uint8_t)(*row_in >> 32);
        *(row_out++) = (uint8_t)(*(row_in++));
        *(row_out++) = (uint8_t)(*(row_in++) >> 32);
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 128, 64, PREMUL8,       LINEAR,
                     123,   24,  8, PREMUL8,       COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint64_t t [2];
        uint8_t alpha = (uint8_t)(row_in [1]);
        unpremul_p8l_to_ul_128bpp (row_in, t, alpha);
        to_srgb_pixel_xxxa_128bpp (row_in, t);
        *(row_out++) = (uint8_t)(t [0] >> 32);
        *(row_out++) = (uint8_t)(t [0]);
        *(row_out++) = (uint8_t)(t [1] >> 32);
        row_in += 2;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 128, 64, PREMUL8,       COMPRESSED,
                     123,   24,  8, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint64_t t [2];
        uint8_t alpha = (uint8_t)(row_in [1]);
        unpremul_p8_to_u_128bpp (row_in, t, alpha);
        t [1] = (t [1] & 0xffffffff00000000ULL) | alpha;
        *(row_out++) = (uint8_t)(t [0] >> 32);
        *(row_out++) = (uint8_t)(t [0]);
        *(row_out++) = (uint8_t)(t [1] >> 32);
        row_in += 2;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 128, 64, PREMUL8,       LINEAR,
                     123,   24,  8, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint64_t t [2];
        uint8_t alpha = (uint8_t)(row_in [1]);
        unpremul_p8l_to_ul_128bpp (row_in, t, alpha);
        to_srgb_pixel_xxxa_128bpp (t, t);
        t [1] = (t [1] & 0xffffffff00000000ULL) | alpha;
        *(row_out++) = (uint8_t)(t [0] >> 32);
        *(row_out++) = (uint8_t)(t [0]);
        *(row_out++) = (uint8_t)(t [1] >> 32);
        row_in += 2;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 128, 64, PREMUL16,      COMPRESSED,
                     123,   24,  8, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint64_t t [2];
        uint8_t alpha = (uint8_t)(row_in [1] >> 8);
        unpremul_p16_to_u_128bpp (row_in, t, alpha);
        t [1] = (t [1] & 0xffffffff00000000ULL) | alpha;
        *(row_out++) = (uint8_t)(t [0] >> 32);
        *(row_out++) = (uint8_t)(t [0]);
        *(row_out++) = (uint8_t)(t [1] >> 32);
        row_in += 2;
    } \
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 128, 64, PREMUL16,      LINEAR,
                     123,   24,  8, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint64_t t [2];
        uint8_t alpha = (uint8_t)(row_in [1] >> 8);
        unpremul_p16_to_u_128bpp (row_in, t, alpha);
        to_srgb_pixel_xxxa_128bpp (t, t);
        t [1] = (t [1] & 0xffffffff00000000ULL) | alpha;
        *(row_out++) = (uint8_t)(t [0] >> 32);
        *(row_out++) = (uint8_t)(t [0]);
        *(row_out++) = (uint8_t)(t [1] >> 32);
        row_in += 2;
    } \
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 128, 64, PREMUL8,       COMPRESSED,
                     321,   24,  8, PREMUL8,       COMPRESSED) {
    while (row_out != row_out_max)
    {
        *(row_out++) = (uint8_t)(row_in [1] >> 32);
        *(row_out++) = (uint8_t)(row_in [0]);
        *(row_out++) = (uint8_t)(row_in [0] >> 32);
        row_in += 2;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 128, 64, PREMUL8,       LINEAR,
                     321,   24,  8, PREMUL8,       COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint64_t t [2];
        uint8_t alpha = (uint8_t)(row_in [1]);
        unpremul_p8l_to_ul_128bpp (row_in, t, alpha);
        to_srgb_pixel_xxxa_128bpp (t, t);
        *(row_out++) = (uint8_t)(t [1] >> 32);
        *(row_out++) = (uint8_t)(t [0]);
        *(row_out++) = (uint8_t)(t [0] >> 32);
        row_in += 2;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 128, 64, PREMUL8,       COMPRESSED,
                     321,   24,  8, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint64_t t [2];
        uint8_t alpha = (uint8_t)(row_in [1]);
        unpremul_p8_to_u_128bpp (row_in, t, alpha);
        t [1] = (t [1] & 0xffffffff00000000ULL) | alpha;
        *(row_out++) = (uint8_t)(t [1] >> 32);
        *(row_out++) = (uint8_t)(t [0]);
        *(row_out++) = (uint8_t)(t [0] >> 32);
        row_in += 2;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 128, 64, PREMUL8,       LINEAR,
                     321,   24,  8, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint64_t t [2];
        uint8_t alpha = (uint8_t)(row_in [1]);
        unpremul_p8l_to_ul_128bpp (row_in, t, alpha);
        to_srgb_pixel_xxxa_128bpp (t, t);
        t [1] = (t [1] & 0xffffffff00000000ULL) | alpha;
        *(row_out++) = (uint8_t)(t [1] >> 32);
        *(row_out++) = (uint8_t)(t [0]);
        *(row_out++) = (uint8_t)(t [0] >> 32);
        row_in += 2;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 128, 64, PREMUL16,      COMPRESSED,
                     321,   24,  8, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint64_t t [2];
        uint8_t alpha = (uint8_t)(row_in [1] >> 8);
        unpremul_p16_to_u_128bpp (row_in, t, alpha);
        t [1] = (t [1] & 0xffffffff00000000ULL) | alpha;
        *(row_out++) = (uint8_t)(t [1] >> 32);
        *(row_out++) = (uint8_t)(t [0]);
        *(row_out++) = (uint8_t)(t [0] >> 32);
        row_in += 2;
    }
} SMOL_REPACK_ROW_DEF_END

SMOL_REPACK_ROW_DEF (1234, 128, 64, PREMUL16,      LINEAR,
                     321,   24,  8, UNASSOCIATED,  COMPRESSED) {
    while (row_out != row_out_max)
    {
        uint64_t t [2];
        uint8_t alpha = (uint8_t)(row_in [1] >> 8);
        unpremul_p16_to_u_128bpp (row_in, t, alpha);
        to_srgb_pixel_xxxa_128bpp (t, t);
        t [1] = (t [1] & 0xffffffff00000000ULL) | alpha;
        *(row_out++) = (uint8_t)(t [1] >> 32);
        *(row_out++) = (uint8_t)(t [0]);
        *(row_out++) = (uint8_t)(t [0] >> 32);
        row_in += 2;
    }
} SMOL_REPACK_ROW_DEF_END

#define DEF_REPACK_FROM_1234_128BPP_TO_32BPP(a, b, c, d) \
    SMOL_REPACK_ROW_DEF (1234,       128, 64, PREMUL8,       COMPRESSED, \
                         a##b##c##d,  32, 32, PREMUL8,       COMPRESSED) { \
        while (row_out != row_out_max) \
        { \
            *(row_out++) = PACK_FROM_1234_128BPP (row_in, a, b, c, d); \
            row_in += 2; \
        } \
    } SMOL_REPACK_ROW_DEF_END \
    SMOL_REPACK_ROW_DEF (1234,       128, 64, PREMUL8,       LINEAR, \
                         a##b##c##d,  32, 32, PREMUL8,       COMPRESSED) { \
        while (row_out != row_out_max) \
        { \
            uint64_t t [2]; \
            uint8_t alpha = (uint8_t)(row_in [1]); \
            unpremul_p8l_to_ul_128bpp (row_in, t, alpha); \
            to_srgb_pixel_xxxa_128bpp (t, t); \
            premul_u_to_p8_128bpp (t, alpha); \
            t [1] = (t [1] & 0xffffffff00000000ULL) | alpha; \
            *(row_out++) = PACK_FROM_1234_128BPP (t, a, b, c, d); \
            row_in += 2; \
        } \
    } SMOL_REPACK_ROW_DEF_END \
    SMOL_REPACK_ROW_DEF (1234,       128, 64, PREMUL8,       COMPRESSED, \
                         a##b##c##d,  32, 32, UNASSOCIATED,  COMPRESSED) { \
        while (row_out != row_out_max) \
        { \
            uint64_t t [2]; \
            uint8_t alpha = (uint8_t)(row_in [1]); \
            unpremul_p8_to_u_128bpp (row_in, t, alpha); \
            t [1] = (t [1] & 0xffffffff00000000ULL) | alpha; \
            *(row_out++) = PACK_FROM_1234_128BPP (t, a, b, c, d); \
            row_in += 2; \
        } \
    } SMOL_REPACK_ROW_DEF_END \
    SMOL_REPACK_ROW_DEF (1234,       128, 64, PREMUL8,       LINEAR, \
                         a##b##c##d,  32, 32, UNASSOCIATED,  COMPRESSED) { \
        while (row_out != row_out_max) \
        { \
            uint64_t t [2]; \
            uint8_t alpha = (uint8_t)(row_in [1]); \
            unpremul_p8l_to_ul_128bpp (row_in, t, alpha); \
            to_srgb_pixel_xxxa_128bpp (t, t); \
            t [1] = (t [1] & 0xffffffff00000000ULL) | alpha; \
            *(row_out++) = PACK_FROM_1234_128BPP (t, a, b, c, d); \
            row_in += 2; \
        } \
    } SMOL_REPACK_ROW_DEF_END \
    SMOL_REPACK_ROW_DEF (1234,       128, 64, PREMUL16,      COMPRESSED, \
                         a##b##c##d,  32, 32, UNASSOCIATED,  COMPRESSED) { \
        while (row_out != row_out_max) \
        { \
            uint64_t t [2]; \
            uint8_t alpha = (uint8_t)(row_in [1] >> 8); \
            unpremul_p16_to_u_128bpp (row_in, t, alpha); \
            t [1] = (t [1] & 0xffffffff00000000ULL) | alpha; \
            *(row_out++) = PACK_FROM_1234_128BPP (t, a, b, c, d); \
            row_in += 2; \
        } \
    } SMOL_REPACK_ROW_DEF_END \
    SMOL_REPACK_ROW_DEF (1234,       128, 64, PREMUL16,      LINEAR, \
                         a##b##c##d,  32, 32, UNASSOCIATED,  COMPRESSED) { \
        while (row_out != row_out_max) \
        { \
            uint64_t t [2]; \
            uint8_t alpha = (uint8_t)(row_in [1] >> 8); \
            unpremul_p16l_to_ul_128bpp (row_in, t, alpha); \
            to_srgb_pixel_xxxa_128bpp (t, t); \
            t [1] = (t [1] & 0xffffffff00000000ULL) | alpha; \
            *(row_out++) = PACK_FROM_1234_128BPP (t, a, b, c, d); \
            row_in += 2; \
        } \
    } SMOL_REPACK_ROW_DEF_END

DEF_REPACK_FROM_1234_128BPP_TO_32BPP (1, 2, 3, 4)
DEF_REPACK_FROM_1234_128BPP_TO_32BPP (3, 2, 1, 4)
DEF_REPACK_FROM_1234_128BPP_TO_32BPP (4, 1, 2, 3)
DEF_REPACK_FROM_1234_128BPP_TO_32BPP (4, 3, 2, 1)

/* -------------- *
 * Filter helpers *
 * -------------- */

static SMOL_INLINE const char *
inrow_ofs_to_pointer (const SmolScaleCtx *scale_ctx,
                      uint32_t inrow_ofs)
{
    return scale_ctx->pixels_in + scale_ctx->rowstride_in * inrow_ofs;
}

static SMOL_INLINE uint64_t
weight_pixel_64bpp (uint64_t p,
                    uint16_t w)
{
    return ((p * w) >> 8) & 0x00ff00ff00ff00ff;
}

/* p and out may be the same address */
static SMOL_INLINE void
weight_pixel_128bpp (uint64_t *p,
                     uint64_t *out,
                     uint16_t w)
{
    out [0] = ((p [0] * w) >> 8) & 0x00ffffff00ffffffULL;
    out [1] = ((p [1] * w) >> 8) & 0x00ffffff00ffffffULL;
}

static SMOL_INLINE void
sum_parts_64bpp (const uint64_t ** SMOL_RESTRICT parts_in,
                 uint64_t * SMOL_RESTRICT accum,
                 uint32_t n)
{
    const uint64_t * SMOL_RESTRICT pp = *parts_in;
    const uint64_t *pp_end;

    SMOL_ASSUME_ALIGNED_TO (pp, const uint64_t *, sizeof (uint64_t));

    for (pp_end = pp + n; pp < pp_end; pp++)
    {
        *accum += *pp;
    }

    *parts_in = pp;
}

static SMOL_INLINE void
sum_parts_128bpp (const uint64_t ** SMOL_RESTRICT parts_in,
                  uint64_t * SMOL_RESTRICT accum,
                  uint32_t n)
{
    const uint64_t * SMOL_RESTRICT pp = *parts_in;
    const uint64_t *pp_end;

    SMOL_ASSUME_ALIGNED_TO (pp, const uint64_t *, sizeof (uint64_t) * 2);

    for (pp_end = pp + n * 2; pp < pp_end; )
    {
        accum [0] += *(pp++);
        accum [1] += *(pp++);
    }

    *parts_in = pp;
}

static SMOL_INLINE uint64_t
scale_64bpp (uint64_t accum,
             uint64_t multiplier)
{
    uint64_t a, b;

    /* Average the inputs */
    a = ((accum & 0x0000ffff0000ffffULL) * multiplier
         + (SMOL_BOXES_MULTIPLIER / 2) + ((SMOL_BOXES_MULTIPLIER / 2) << 32)) / SMOL_BOXES_MULTIPLIER;
    b = (((accum & 0xffff0000ffff0000ULL) >> 16) * multiplier
         + (SMOL_BOXES_MULTIPLIER / 2) + ((SMOL_BOXES_MULTIPLIER / 2) << 32)) / SMOL_BOXES_MULTIPLIER;

    /* Return pixel */
    return (a & 0x000000ff000000ffULL) | ((b & 0x000000ff000000ffULL) << 16);
}

static SMOL_INLINE uint64_t
scale_128bpp_half (uint64_t accum,
                   uint64_t multiplier)
{
    uint64_t a, b;

    a = accum & 0x00000000ffffffffULL;
    a = (a * multiplier + SMOL_BOXES_MULTIPLIER / 2) / SMOL_BOXES_MULTIPLIER;

    b = (accum & 0xffffffff00000000ULL) >> 32;
    b = (b * multiplier + SMOL_BOXES_MULTIPLIER / 2) / SMOL_BOXES_MULTIPLIER;

    return (a & 0x000000000000ffffULL)
           | ((b & 0x000000000000ffffULL) << 32);
}

static SMOL_INLINE void
scale_and_store_128bpp (const uint64_t * SMOL_RESTRICT accum,
                        uint64_t multiplier,
                        uint64_t ** SMOL_RESTRICT row_parts_out)
{
    *(*row_parts_out)++ = scale_128bpp_half (accum [0], multiplier);
    *(*row_parts_out)++ = scale_128bpp_half (accum [1], multiplier);
}

static void
add_parts (const uint64_t * SMOL_RESTRICT parts_in,
           uint64_t * SMOL_RESTRICT parts_acc_out,
           uint32_t n)
{
    const uint64_t *parts_in_max = parts_in + n;

    SMOL_ASSUME_ALIGNED (parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (parts_acc_out, uint64_t *);

    while (parts_in < parts_in_max)
        *(parts_acc_out++) += *(parts_in++);
}

/* ------------------ *
 * Horizontal scaling *
 * ------------------ */

#define DEF_INTERP_HORIZONTAL_BILINEAR(n_halvings) \
static void \
interp_horizontal_bilinear_##n_halvings##h_64bpp (const SmolScaleCtx *scale_ctx, \
                                                  const uint64_t * SMOL_RESTRICT row_parts_in, \
                                                  uint64_t * SMOL_RESTRICT row_parts_out) \
{ \
    const uint16_t * SMOL_RESTRICT precalc_x = scale_ctx->precalc_x; \
    uint64_t *row_parts_out_max = row_parts_out + scale_ctx->width_out; \
    uint64_t p, q; \
    uint64_t F; \
    int i; \
\
    SMOL_ASSUME_ALIGNED (row_parts_in, const uint64_t *); \
    SMOL_ASSUME_ALIGNED (row_parts_out, uint64_t *); \
\
    do \
    { \
        uint64_t accum = 0; \
\
        for (i = 0; i < (1 << (n_halvings)); i++) \
        { \
            row_parts_in += *(precalc_x++); \
            F = *(precalc_x++); \
\
            p = *row_parts_in; \
            q = *(row_parts_in + 1); \
\
            accum += ((((p - q) * F) >> 8) + q) & 0x00ff00ff00ff00ffULL; \
            } \
        *(row_parts_out++) = ((accum) >> (n_halvings)) & 0x00ff00ff00ff00ffULL; \
    } \
    while (row_parts_out != row_parts_out_max); \
} \
\
static void \
interp_horizontal_bilinear_##n_halvings##h_128bpp (const SmolScaleCtx *scale_ctx, \
                                                   const uint64_t * SMOL_RESTRICT row_parts_in, \
                                                   uint64_t * SMOL_RESTRICT row_parts_out) \
{ \
    const uint16_t * SMOL_RESTRICT precalc_x = scale_ctx->precalc_x; \
    uint64_t *row_parts_out_max = row_parts_out + scale_ctx->width_out * 2; \
    uint64_t p, q; \
    uint64_t F; \
    int i; \
\
    SMOL_ASSUME_ALIGNED (row_parts_in, const uint64_t *); \
    SMOL_ASSUME_ALIGNED (row_parts_out, uint64_t *); \
\
    do \
    { \
        uint64_t accum [2] = { 0 }; \
         \
        for (i = 0; i < (1 << (n_halvings)); i++) \
        { \
            row_parts_in += *(precalc_x++) * 2; \
            F = *(precalc_x++); \
\
            p = row_parts_in [0]; \
            q = row_parts_in [2]; \
\
            accum [0] += ((((p - q) * F) >> 8) + q) & 0x00ffffff00ffffffULL; \
\
            p = row_parts_in [1]; \
            q = row_parts_in [3]; \
\
            accum [1] += ((((p - q) * F) >> 8) + q) & 0x00ffffff00ffffffULL; \
            } \
        *(row_parts_out++) = ((accum [0]) >> (n_halvings)) & 0x00ffffff00ffffffULL; \
        *(row_parts_out++) = ((accum [1]) >> (n_halvings)) & 0x00ffffff00ffffffULL; \
    } \
    while (row_parts_out != row_parts_out_max); \
}

static void
interp_horizontal_bilinear_0h_64bpp (const SmolScaleCtx *scale_ctx,
                                     const uint64_t * SMOL_RESTRICT row_parts_in,
                                     uint64_t * SMOL_RESTRICT row_parts_out)
{
    const uint16_t * SMOL_RESTRICT precalc_x = scale_ctx->precalc_x;
    uint64_t * SMOL_RESTRICT row_parts_out_max = row_parts_out + scale_ctx->width_out;
    uint64_t p, q;
    uint64_t F;

    SMOL_ASSUME_ALIGNED (row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (row_parts_out, uint64_t *);

    do
    {
        row_parts_in += *(precalc_x++);
        F = *(precalc_x++);

        p = *row_parts_in;
        q = *(row_parts_in + 1);

        *(row_parts_out++) = ((((p - q) * F) >> 8) + q) & 0x00ff00ff00ff00ffULL;
    }
    while (row_parts_out != row_parts_out_max);
}

static void
interp_horizontal_bilinear_0h_128bpp (const SmolScaleCtx *scale_ctx,
                                      const uint64_t * SMOL_RESTRICT row_parts_in,
                                      uint64_t * SMOL_RESTRICT row_parts_out)
{
    const uint16_t * SMOL_RESTRICT precalc_x = scale_ctx->precalc_x;
    uint64_t * SMOL_RESTRICT row_parts_out_max = row_parts_out + scale_ctx->width_out * 2;
    uint64_t p, q;
    uint64_t F;

    SMOL_ASSUME_ALIGNED (row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (row_parts_out, uint64_t *);

    do
    {
        row_parts_in += *(precalc_x++) * 2;
        F = *(precalc_x++);

        p = row_parts_in [0];
        q = row_parts_in [2];

        *(row_parts_out++) = ((((p - q) * F) >> 8) + q) & 0x00ffffff00ffffffULL;

        p = row_parts_in [1];
        q = row_parts_in [3];

        *(row_parts_out++) = ((((p - q) * F) >> 8) + q) & 0x00ffffff00ffffffULL;
    }
    while (row_parts_out != row_parts_out_max);
}

DEF_INTERP_HORIZONTAL_BILINEAR(1)
DEF_INTERP_HORIZONTAL_BILINEAR(2)
DEF_INTERP_HORIZONTAL_BILINEAR(3)
DEF_INTERP_HORIZONTAL_BILINEAR(4)
DEF_INTERP_HORIZONTAL_BILINEAR(5)
DEF_INTERP_HORIZONTAL_BILINEAR(6)

static void
interp_horizontal_boxes_64bpp (const SmolScaleCtx *scale_ctx,
                               const uint64_t *row_parts_in,
                               uint64_t * SMOL_RESTRICT row_parts_out)
{
    const uint64_t * SMOL_RESTRICT pp;
    const uint16_t *precalc_x = scale_ctx->precalc_x;
    uint64_t *row_parts_out_max = row_parts_out + scale_ctx->width_out - 1;
    uint64_t accum = 0;
    uint64_t p, q, r, s;
    uint32_t n;
    uint64_t F;

    SMOL_ASSUME_ALIGNED (row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (row_parts_out, uint64_t *);

    pp = row_parts_in;
    p = weight_pixel_64bpp (*(pp++), 256);
    n = *(precalc_x++);

    while (row_parts_out != row_parts_out_max)
    {
        sum_parts_64bpp ((const uint64_t ** SMOL_RESTRICT) &pp, &accum, n);

        F = *(precalc_x++);
        n = *(precalc_x++);

        r = *(pp++);
        s = r * F;

        q = (s >> 8) & 0x00ff00ff00ff00ffULL;

        accum += p + q;

        /* (255 * r) - (F * r) */
        p = (((r << 8) - r - s) >> 8) & 0x00ff00ff00ff00ffULL;

        *(row_parts_out++) = scale_64bpp (accum, scale_ctx->span_mul_x);
        accum = 0;
    }

    /* Final box optionally features the rightmost fractional pixel */

    sum_parts_64bpp ((const uint64_t ** SMOL_RESTRICT) &pp, &accum, n);

    q = 0;
    F = *(precalc_x);
    if (F > 0)
        q = weight_pixel_64bpp (*(pp), (uint16_t)F);

    accum += p + q;
    *(row_parts_out++) = scale_64bpp (accum, scale_ctx->span_mul_x);
}

static void
interp_horizontal_boxes_128bpp (const SmolScaleCtx *scale_ctx,
                                const uint64_t *row_parts_in,
                                uint64_t * SMOL_RESTRICT row_parts_out)
{
    const uint64_t * SMOL_RESTRICT pp;
    const uint16_t *precalc_x = scale_ctx->precalc_x;
    uint64_t *row_parts_out_max = row_parts_out + (scale_ctx->width_out - 1) * 2;
    uint64_t accum [2] = { 0, 0 };
    uint64_t p [2], q [2], r [2], s [2];
    uint32_t n;
    uint64_t F;

    SMOL_ASSUME_ALIGNED (row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (row_parts_out, uint64_t *);

    pp = row_parts_in;

    p [0] = *(pp++);
    p [1] = *(pp++);
    weight_pixel_128bpp (p, p, 256);

    n = *(precalc_x++);

    while (row_parts_out != row_parts_out_max)
    {
        sum_parts_128bpp ((const uint64_t ** SMOL_RESTRICT) &pp, accum, n);

        F = *(precalc_x++);
        n = *(precalc_x++);

        r [0] = *(pp++);
        r [1] = *(pp++);

        s [0] = r [0] * F;
        s [1] = r [1] * F;

        q [0] = (s [0] >> 8) & 0x00ffffff00ffffff;
        q [1] = (s [1] >> 8) & 0x00ffffff00ffffff;

        accum [0] += p [0] + q [0];
        accum [1] += p [1] + q [1];

        p [0] = (((r [0] << 8) - r [0] - s [0]) >> 8) & 0x00ffffff00ffffff;
        p [1] = (((r [1] << 8) - r [1] - s [1]) >> 8) & 0x00ffffff00ffffff;

        scale_and_store_128bpp (accum,
                                scale_ctx->span_mul_x,
                                (uint64_t ** SMOL_RESTRICT) &row_parts_out);

        accum [0] = 0;
        accum [1] = 0;
    }

    /* Final box optionally features the rightmost fractional pixel */

    sum_parts_128bpp ((const uint64_t ** SMOL_RESTRICT) &pp, accum, n);

    q [0] = 0;
    q [1] = 0;

    F = *(precalc_x);
    if (F > 0)
    {
        q [0] = *(pp++);
        q [1] = *(pp++);
        weight_pixel_128bpp (q, q, (uint16_t)F);
    }

    accum [0] += p [0] + q [0];
    accum [1] += p [1] + q [1];

    scale_and_store_128bpp (accum,
                            scale_ctx->span_mul_x,
                            (uint64_t ** SMOL_RESTRICT) &row_parts_out);
}

static void
interp_horizontal_one_64bpp (const SmolScaleCtx *scale_ctx,
                             const uint64_t * SMOL_RESTRICT row_parts_in,
                             uint64_t * SMOL_RESTRICT row_parts_out)
{
    uint64_t *row_parts_out_max = row_parts_out + scale_ctx->width_out;
    uint64_t part;

    SMOL_ASSUME_ALIGNED (row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (row_parts_out, uint64_t *);

    part = *row_parts_in;
    while (row_parts_out != row_parts_out_max)
        *(row_parts_out++) = part;
}

static void
interp_horizontal_one_128bpp (const SmolScaleCtx *scale_ctx,
                              const uint64_t * SMOL_RESTRICT row_parts_in,
                              uint64_t * SMOL_RESTRICT row_parts_out)
{
    uint64_t *row_parts_out_max = row_parts_out + scale_ctx->width_out * 2;

    SMOL_ASSUME_ALIGNED (row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (row_parts_out, uint64_t *);

    while (row_parts_out != row_parts_out_max)
    {
        *(row_parts_out++) = row_parts_in [0];
        *(row_parts_out++) = row_parts_in [1];
    }
}

static void
interp_horizontal_copy_64bpp (const SmolScaleCtx *scale_ctx,
                              const uint64_t * SMOL_RESTRICT row_parts_in,
                              uint64_t * SMOL_RESTRICT row_parts_out)
{
    SMOL_ASSUME_ALIGNED (row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (row_parts_out, uint64_t *);

    memcpy (row_parts_out, row_parts_in, scale_ctx->width_out * sizeof (uint64_t));
}

static void
interp_horizontal_copy_128bpp (const SmolScaleCtx *scale_ctx,
                               const uint64_t * SMOL_RESTRICT row_parts_in,
                               uint64_t * SMOL_RESTRICT row_parts_out)
{
    SMOL_ASSUME_ALIGNED (row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (row_parts_out, uint64_t *);

    memcpy (row_parts_out, row_parts_in, scale_ctx->width_out * 2 * sizeof (uint64_t));
}

static void
scale_horizontal (const SmolScaleCtx *scale_ctx,
                  SmolVerticalCtx *vertical_ctx,
                  const char *row_in,
                  uint64_t *row_parts_out)
{
    uint64_t * SMOL_RESTRICT unpacked_in;

    unpacked_in = vertical_ctx->parts_row [3];

    /* 32-bit unpackers need 32-bit alignment */
    if ((((uintptr_t) row_in) & 3)
        && scale_ctx->pixel_type_in != SMOL_PIXEL_RGB8
        && scale_ctx->pixel_type_in != SMOL_PIXEL_BGR8)
    {
        if (!vertical_ctx->in_aligned) {
            vertical_ctx->in_aligned =
                smol_alloc_aligned (scale_ctx->width_in * sizeof (uint32_t),
                                    &vertical_ctx->in_aligned_storage);
            vertical_ctx->in_aligned_storage = vertical_ctx->in_aligned;
        }
        memcpy (vertical_ctx->in_aligned, row_in, scale_ctx->width_in * sizeof (uint32_t));
        row_in = (const char *) vertical_ctx->in_aligned;
    }

    scale_ctx->unpack_row_func ((const void *) row_in,
                                (void *)unpacked_in,
                                scale_ctx->width_in);
    scale_ctx->hfilter_func (scale_ctx,
                             unpacked_in,
                             row_parts_out);
}

/* ---------------- *
 * Vertical scaling *
 * ---------------- */

static void
update_vertical_ctx_bilinear (const SmolScaleCtx *scale_ctx,
                              SmolVerticalCtx *vertical_ctx,
                              uint32_t outrow_index)
{
    uint32_t new_in_ofs = scale_ctx->precalc_y [outrow_index * 2];

    if (new_in_ofs == vertical_ctx->in_ofs)
        return;

    if (new_in_ofs == vertical_ctx->in_ofs + 1)
    {
        uint64_t *t = vertical_ctx->parts_row [0];
        vertical_ctx->parts_row [0] = vertical_ctx->parts_row [1];
        vertical_ctx->parts_row [1] = t;

        scale_horizontal (scale_ctx,
                          vertical_ctx,
                          inrow_ofs_to_pointer (scale_ctx, new_in_ofs + 1),
                          vertical_ctx->parts_row [1]);
    }
    else
    {
        scale_horizontal (scale_ctx,
                          vertical_ctx,
                          inrow_ofs_to_pointer (scale_ctx, new_in_ofs),
                          vertical_ctx->parts_row [0]);
        scale_horizontal (scale_ctx,
                          vertical_ctx,
                          inrow_ofs_to_pointer (scale_ctx, new_in_ofs + 1),
                          vertical_ctx->parts_row [1]);
    }

    vertical_ctx->in_ofs = new_in_ofs;
}

static void
interp_vertical_bilinear_store_64bpp (uint64_t F,
                                      const uint64_t * SMOL_RESTRICT top_row_parts_in,
                                      const uint64_t * SMOL_RESTRICT bottom_row_parts_in,
                                      uint64_t * SMOL_RESTRICT parts_out,
                                      uint32_t width)
{
    uint64_t *parts_out_last = parts_out + width;

    SMOL_ASSUME_ALIGNED (top_row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (bottom_row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (parts_out, uint64_t *);

    do
    {
        uint64_t p, q;

        p = *(top_row_parts_in++);
        q = *(bottom_row_parts_in++);

        *(parts_out++) = ((((p - q) * F) >> 8) + q) & 0x00ff00ff00ff00ffULL;
    }
    while (parts_out != parts_out_last);
}

static void
interp_vertical_bilinear_add_64bpp (uint64_t F,
                                    const uint64_t * SMOL_RESTRICT top_row_parts_in,
                                    const uint64_t * SMOL_RESTRICT bottom_row_parts_in,
                                    uint64_t * SMOL_RESTRICT accum_out,
                                    uint32_t width)
{
    uint64_t *accum_out_last = accum_out + width;

    SMOL_ASSUME_ALIGNED (top_row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (bottom_row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (accum_out, uint64_t *);

    do
    {
        uint64_t p, q;

        p = *(top_row_parts_in++);
        q = *(bottom_row_parts_in++);

        *(accum_out++) += ((((p - q) * F) >> 8) + q) & 0x00ff00ff00ff00ffULL;
    }
    while (accum_out != accum_out_last);
}

static void
interp_vertical_bilinear_store_128bpp (uint64_t F,
                                       const uint64_t * SMOL_RESTRICT top_row_parts_in,
                                       const uint64_t * SMOL_RESTRICT bottom_row_parts_in,
                                       uint64_t * SMOL_RESTRICT parts_out,
                                       uint32_t width)
{
    uint64_t *parts_out_last = parts_out + width;

    SMOL_ASSUME_ALIGNED (top_row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (bottom_row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (parts_out, uint64_t *);

    do
    {
        uint64_t p, q;

        p = *(top_row_parts_in++);
        q = *(bottom_row_parts_in++);

        *(parts_out++) = ((((p - q) * F) >> 8) + q) & 0x00ffffff00ffffffULL;
    }
    while (parts_out != parts_out_last);
}

static void
interp_vertical_bilinear_add_128bpp (uint64_t F,
                                     const uint64_t * SMOL_RESTRICT top_row_parts_in,
                                     const uint64_t * SMOL_RESTRICT bottom_row_parts_in,
                                     uint64_t * SMOL_RESTRICT accum_out,
                                     uint32_t width)
{
    uint64_t *accum_out_last = accum_out + width;

    SMOL_ASSUME_ALIGNED (top_row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (bottom_row_parts_in, const uint64_t *);
    SMOL_ASSUME_ALIGNED (accum_out, uint64_t *);

    do
    {
        uint64_t p, q;

        p = *(top_row_parts_in++);
        q = *(bottom_row_parts_in++);

        *(accum_out++) += ((((p - q) * F) >> 8) + q) & 0x00ffffff00ffffffULL;
    }
    while (accum_out != accum_out_last);
}

#define DEF_INTERP_VERTICAL_BILINEAR_FINAL(n_halvings) \
static void \
interp_vertical_bilinear_final_##n_halvings##h_64bpp (uint64_t F, \
                                                      const uint64_t * SMOL_RESTRICT top_row_parts_in, \
                                                      const uint64_t * SMOL_RESTRICT bottom_row_parts_in, \
                                                      uint64_t * SMOL_RESTRICT accum_inout, \
                                                      uint32_t width) \
{ \
    uint64_t *accum_inout_last = accum_inout + width; \
\
    SMOL_ASSUME_ALIGNED (top_row_parts_in, const uint64_t *); \
    SMOL_ASSUME_ALIGNED (bottom_row_parts_in, const uint64_t *); \
    SMOL_ASSUME_ALIGNED (accum_inout, uint64_t *); \
\
    do \
    { \
        uint64_t p, q; \
\
        p = *(top_row_parts_in++); \
        q = *(bottom_row_parts_in++); \
\
        p = ((((p - q) * F) >> 8) + q) & 0x00ff00ff00ff00ffULL; \
        p = ((p + *accum_inout) >> n_halvings) & 0x00ff00ff00ff00ffULL; \
\
        *(accum_inout++) = p; \
    } \
    while (accum_inout != accum_inout_last); \
} \
\
static void \
interp_vertical_bilinear_final_##n_halvings##h_128bpp (uint64_t F, \
                                                       const uint64_t * SMOL_RESTRICT top_row_parts_in, \
                                                       const uint64_t * SMOL_RESTRICT bottom_row_parts_in, \
                                                       uint64_t * SMOL_RESTRICT accum_inout, \
                                                       uint32_t width) \
{ \
    uint64_t *accum_inout_last = accum_inout + width; \
\
    SMOL_ASSUME_ALIGNED (top_row_parts_in, const uint64_t *); \
    SMOL_ASSUME_ALIGNED (bottom_row_parts_in, const uint64_t *); \
    SMOL_ASSUME_ALIGNED (accum_inout, uint64_t *); \
\
    do \
    { \
        uint64_t p, q; \
\
        p = *(top_row_parts_in++); \
        q = *(bottom_row_parts_in++); \
\
        p = ((((p - q) * F) >> 8) + q) & 0x00ffffff00ffffffULL; \
        p = ((p + *accum_inout) >> n_halvings) & 0x00ffffff00ffffffULL; \
\
        *(accum_inout++) = p; \
    } \
    while (accum_inout != accum_inout_last); \
}

#define DEF_SCALE_OUTROW_BILINEAR(n_halvings) \
static void \
scale_outrow_bilinear_##n_halvings##h_64bpp (const SmolScaleCtx *scale_ctx, \
                                             SmolVerticalCtx *vertical_ctx, \
                                             uint32_t outrow_index, \
                                             uint32_t *row_out) \
{ \
    uint32_t bilin_index = outrow_index << (n_halvings); \
    unsigned int i; \
\
    update_vertical_ctx_bilinear (scale_ctx, vertical_ctx, bilin_index); \
    interp_vertical_bilinear_store_64bpp (scale_ctx->precalc_y [bilin_index * 2 + 1], \
                                          vertical_ctx->parts_row [0], \
                                          vertical_ctx->parts_row [1], \
                                          vertical_ctx->parts_row [2], \
                                          scale_ctx->width_out); \
    bilin_index++; \
\
    for (i = 0; i < (1 << (n_halvings)) - 2; i++) \
    { \
        update_vertical_ctx_bilinear (scale_ctx, vertical_ctx, bilin_index); \
        interp_vertical_bilinear_add_64bpp (scale_ctx->precalc_y [bilin_index * 2 + 1], \
                                            vertical_ctx->parts_row [0], \
                                            vertical_ctx->parts_row [1], \
                                            vertical_ctx->parts_row [2], \
                                            scale_ctx->width_out); \
        bilin_index++; \
    } \
\
    update_vertical_ctx_bilinear (scale_ctx, vertical_ctx, bilin_index); \
    interp_vertical_bilinear_final_##n_halvings##h_64bpp (scale_ctx->precalc_y [bilin_index * 2 + 1], \
                                                          vertical_ctx->parts_row [0], \
                                                          vertical_ctx->parts_row [1], \
                                                          vertical_ctx->parts_row [2], \
                                                          scale_ctx->width_out); \
\
    scale_ctx->pack_row_func (vertical_ctx->parts_row [2], row_out, scale_ctx->width_out); \
} \
\
static void \
scale_outrow_bilinear_##n_halvings##h_128bpp (const SmolScaleCtx *scale_ctx, \
                                              SmolVerticalCtx *vertical_ctx, \
                                              uint32_t outrow_index, \
                                              uint32_t *row_out) \
{ \
    uint32_t bilin_index = outrow_index << (n_halvings); \
    unsigned int i; \
\
    update_vertical_ctx_bilinear (scale_ctx, vertical_ctx, bilin_index); \
    interp_vertical_bilinear_store_128bpp (scale_ctx->precalc_y [bilin_index * 2 + 1], \
                                           vertical_ctx->parts_row [0], \
                                           vertical_ctx->parts_row [1], \
                                           vertical_ctx->parts_row [2], \
                                           scale_ctx->width_out * 2); \
    bilin_index++; \
\
    for (i = 0; i < (1 << (n_halvings)) - 2; i++) \
    { \
        update_vertical_ctx_bilinear (scale_ctx, vertical_ctx, bilin_index); \
        interp_vertical_bilinear_add_128bpp (scale_ctx->precalc_y [bilin_index * 2 + 1], \
                                             vertical_ctx->parts_row [0], \
                                             vertical_ctx->parts_row [1], \
                                             vertical_ctx->parts_row [2], \
                                             scale_ctx->width_out * 2); \
        bilin_index++; \
    } \
\
    update_vertical_ctx_bilinear (scale_ctx, vertical_ctx, bilin_index); \
    interp_vertical_bilinear_final_##n_halvings##h_128bpp (scale_ctx->precalc_y [bilin_index * 2 + 1], \
                                                           vertical_ctx->parts_row [0], \
                                                           vertical_ctx->parts_row [1], \
                                                           vertical_ctx->parts_row [2], \
                                                           scale_ctx->width_out * 2); \
\
    scale_ctx->pack_row_func (vertical_ctx->parts_row [2], row_out, scale_ctx->width_out); \
}

static void
scale_outrow_bilinear_0h_64bpp (const SmolScaleCtx *scale_ctx,
                                SmolVerticalCtx *vertical_ctx,
                                uint32_t outrow_index,
                                uint32_t *row_out)
{
    update_vertical_ctx_bilinear (scale_ctx, vertical_ctx, outrow_index);
    interp_vertical_bilinear_store_64bpp (scale_ctx->precalc_y [outrow_index * 2 + 1],
                                          vertical_ctx->parts_row [0],
                                          vertical_ctx->parts_row [1],
                                          vertical_ctx->parts_row [2],
                                          scale_ctx->width_out);
    scale_ctx->pack_row_func (vertical_ctx->parts_row [2], row_out, scale_ctx->width_out);
}

static void
scale_outrow_bilinear_0h_128bpp (const SmolScaleCtx *scale_ctx,
                                 SmolVerticalCtx *vertical_ctx,
                                 uint32_t outrow_index,
                                 uint32_t *row_out)
{
    update_vertical_ctx_bilinear (scale_ctx, vertical_ctx, outrow_index);
    interp_vertical_bilinear_store_128bpp (scale_ctx->precalc_y [outrow_index * 2 + 1],
                                           vertical_ctx->parts_row [0],
                                           vertical_ctx->parts_row [1],
                                           vertical_ctx->parts_row [2],
                                           scale_ctx->width_out * 2);
    scale_ctx->pack_row_func (vertical_ctx->parts_row [2], row_out, scale_ctx->width_out);
}

DEF_INTERP_VERTICAL_BILINEAR_FINAL(1)

static void
scale_outrow_bilinear_1h_64bpp (const SmolScaleCtx *scale_ctx,
                                SmolVerticalCtx *vertical_ctx,
                                uint32_t outrow_index,
                                uint32_t *row_out)
{
    uint32_t bilin_index = outrow_index << 1;

    update_vertical_ctx_bilinear (scale_ctx, vertical_ctx, bilin_index);
    interp_vertical_bilinear_store_64bpp (scale_ctx->precalc_y [bilin_index * 2 + 1],
                                          vertical_ctx->parts_row [0],
                                          vertical_ctx->parts_row [1],
                                          vertical_ctx->parts_row [2],
                                          scale_ctx->width_out);
    bilin_index++;
    update_vertical_ctx_bilinear (scale_ctx, vertical_ctx, bilin_index);
    interp_vertical_bilinear_final_1h_64bpp (scale_ctx->precalc_y [bilin_index * 2 + 1],
                                             vertical_ctx->parts_row [0],
                                             vertical_ctx->parts_row [1],
                                             vertical_ctx->parts_row [2],
                                             scale_ctx->width_out);
    scale_ctx->pack_row_func (vertical_ctx->parts_row [2], row_out, scale_ctx->width_out);
}

static void
scale_outrow_bilinear_1h_128bpp (const SmolScaleCtx *scale_ctx,
                                 SmolVerticalCtx *vertical_ctx,
                                 uint32_t outrow_index,
                                 uint32_t *row_out)
{
    uint32_t bilin_index = outrow_index << 1;

    update_vertical_ctx_bilinear (scale_ctx, vertical_ctx, bilin_index);
    interp_vertical_bilinear_store_128bpp (scale_ctx->precalc_y [bilin_index * 2 + 1],
                                           vertical_ctx->parts_row [0],
                                           vertical_ctx->parts_row [1],
                                           vertical_ctx->parts_row [2],
                                           scale_ctx->width_out * 2);
    bilin_index++;
    update_vertical_ctx_bilinear (scale_ctx, vertical_ctx, bilin_index);
    interp_vertical_bilinear_final_1h_128bpp (scale_ctx->precalc_y [bilin_index * 2 + 1],
                                              vertical_ctx->parts_row [0],
                                              vertical_ctx->parts_row [1],
                                              vertical_ctx->parts_row [2],
                                              scale_ctx->width_out * 2);
    scale_ctx->pack_row_func (vertical_ctx->parts_row [2], row_out, scale_ctx->width_out);
}

DEF_INTERP_VERTICAL_BILINEAR_FINAL(2)
DEF_SCALE_OUTROW_BILINEAR(2)
DEF_INTERP_VERTICAL_BILINEAR_FINAL(3)
DEF_SCALE_OUTROW_BILINEAR(3)
DEF_INTERP_VERTICAL_BILINEAR_FINAL(4)
DEF_SCALE_OUTROW_BILINEAR(4)
DEF_INTERP_VERTICAL_BILINEAR_FINAL(5)
DEF_SCALE_OUTROW_BILINEAR(5)
DEF_INTERP_VERTICAL_BILINEAR_FINAL(6)
DEF_SCALE_OUTROW_BILINEAR(6)

static void
finalize_vertical_64bpp (const uint64_t * SMOL_RESTRICT accums,
                         uint64_t multiplier,
                         uint64_t * SMOL_RESTRICT parts_out,
                         uint32_t n)
{
    uint64_t *parts_out_max = parts_out + n;

    SMOL_ASSUME_ALIGNED (accums, const uint64_t *);
    SMOL_ASSUME_ALIGNED (parts_out, uint64_t *);

    while (parts_out != parts_out_max)
    {
        *(parts_out++) = scale_64bpp (*(accums++), multiplier);
    }
}

static void
weight_edge_row_64bpp (uint64_t *row,
                       uint16_t w,
                       uint32_t n)
{
    uint64_t *row_max = row + n;

    SMOL_ASSUME_ALIGNED (row, uint64_t *);

    while (row != row_max)
    {
        *row = ((*row * w) >> 8) & 0x00ff00ff00ff00ffULL;
        row++;
    }
}

static void
scale_and_weight_edge_rows_box_64bpp (const uint64_t * SMOL_RESTRICT first_row,
                                      uint64_t * SMOL_RESTRICT last_row,
                                      uint64_t * SMOL_RESTRICT accum,
                                      uint16_t w2,
                                      uint32_t n)
{
    const uint64_t *first_row_max = first_row + n;

    SMOL_ASSUME_ALIGNED (first_row, const uint64_t *);
    SMOL_ASSUME_ALIGNED (last_row, uint64_t *);
    SMOL_ASSUME_ALIGNED (accum, uint64_t *);

    while (first_row != first_row_max)
    {
        uint64_t r, s, p, q;

        p = *(first_row++);

        r = *(last_row);
        s = r * w2;
        q = (s >> 8) & 0x00ff00ff00ff00ffULL;
        /* (255 * r) - (F * r) */
        *(last_row++) = (((r << 8) - r - s) >> 8) & 0x00ff00ff00ff00ffULL;

        *(accum++) = p + q;
    }
}

static void
update_vertical_ctx_box_64bpp (const SmolScaleCtx *scale_ctx,
                               SmolVerticalCtx *vertical_ctx,
                               uint32_t ofs_y,
                               uint32_t ofs_y_max,
                               uint16_t w1,
                               uint16_t w2)
{
    /* Old in_ofs is the previous max */
    if (ofs_y == vertical_ctx->in_ofs)
    {
        uint64_t *t = vertical_ctx->parts_row [0];
        vertical_ctx->parts_row [0] = vertical_ctx->parts_row [1];
        vertical_ctx->parts_row [1] = t;
    }
    else
    {
        scale_horizontal (scale_ctx,
                          vertical_ctx,
                          inrow_ofs_to_pointer (scale_ctx, ofs_y),
                          vertical_ctx->parts_row [0]);
        weight_edge_row_64bpp (vertical_ctx->parts_row [0], w1, scale_ctx->width_out);
    }

    /* When w2 == 0, the final inrow may be out of bounds. Don't try to access it in
     * that case. */
    if (w2 || ofs_y_max < scale_ctx->height_in)
    {
        scale_horizontal (scale_ctx,
                          vertical_ctx,
                          inrow_ofs_to_pointer (scale_ctx, ofs_y_max),
                          vertical_ctx->parts_row [1]);
    }
    else
    {
        memset (vertical_ctx->parts_row [1], 0, scale_ctx->width_out * sizeof (uint64_t));
    }

    vertical_ctx->in_ofs = ofs_y_max;
}

static void
scale_outrow_box_64bpp (const SmolScaleCtx *scale_ctx,
                        SmolVerticalCtx *vertical_ctx,
                        uint32_t outrow_index,
                        uint32_t *row_out)
{
    uint32_t ofs_y, ofs_y_max;
    uint16_t w1, w2;

    /* Get the inrow range for this outrow: [ofs_y .. ofs_y_max> */

    ofs_y = scale_ctx->precalc_y [outrow_index * 2];
    ofs_y_max = scale_ctx->precalc_y [(outrow_index + 1) * 2];

    /* Scale the first and last rows, weight them and store in accumulator */

    w1 = (outrow_index == 0) ? 256 : 255 - scale_ctx->precalc_y [outrow_index * 2 - 1];
    w2 = scale_ctx->precalc_y [outrow_index * 2 + 1];

    update_vertical_ctx_box_64bpp (scale_ctx, vertical_ctx, ofs_y, ofs_y_max, w1, w2);

    scale_and_weight_edge_rows_box_64bpp (vertical_ctx->parts_row [0],
                                          vertical_ctx->parts_row [1],
                                          vertical_ctx->parts_row [2],
                                          w2,
                                          scale_ctx->width_out);

    ofs_y++;

    /* Add up whole rows */

    while (ofs_y < ofs_y_max)
    {
        scale_horizontal (scale_ctx,
                          vertical_ctx,
                          inrow_ofs_to_pointer (scale_ctx, ofs_y),
                          vertical_ctx->parts_row [0]);
        add_parts (vertical_ctx->parts_row [0],
                   vertical_ctx->parts_row [2],
                   scale_ctx->width_out);

        ofs_y++;
    }

    finalize_vertical_64bpp (vertical_ctx->parts_row [2],
                             scale_ctx->span_mul_y,
                             vertical_ctx->parts_row [0],
                             scale_ctx->width_out);
    scale_ctx->pack_row_func (vertical_ctx->parts_row [0], row_out, scale_ctx->width_out);
}

static void
finalize_vertical_128bpp (const uint64_t * SMOL_RESTRICT accums,
                          uint64_t multiplier,
                          uint64_t * SMOL_RESTRICT parts_out,
                          uint32_t n)
{
    uint64_t *parts_out_max = parts_out + n * 2;

    SMOL_ASSUME_ALIGNED (accums, const uint64_t *);
    SMOL_ASSUME_ALIGNED (parts_out, uint64_t *);

    while (parts_out != parts_out_max)
    {
        *(parts_out++) = scale_128bpp_half (*(accums++), multiplier);
        *(parts_out++) = scale_128bpp_half (*(accums++), multiplier);
    }
}

static void
weight_row_128bpp (uint64_t *row,
                   uint16_t w,
                   uint32_t n)
{
    uint64_t *row_max = row + (n * 2);

    SMOL_ASSUME_ALIGNED (row, uint64_t *);

    while (row != row_max)
    {
        row [0] = ((row [0] * w) >> 8) & 0x00ffffff00ffffffULL;
        row [1] = ((row [1] * w) >> 8) & 0x00ffffff00ffffffULL;
        row += 2;
    }
}

static void
scale_outrow_box_128bpp (const SmolScaleCtx *scale_ctx,
                         SmolVerticalCtx *vertical_ctx,
                         uint32_t outrow_index,
                         uint32_t *row_out)
{
    uint32_t ofs_y, ofs_y_max;
    uint16_t w;

    /* Get the inrow range for this outrow: [ofs_y .. ofs_y_max> */

    ofs_y = scale_ctx->precalc_y [outrow_index * 2];
    ofs_y_max = scale_ctx->precalc_y [(outrow_index + 1) * 2];

    /* Scale the first inrow and store it */

    scale_horizontal (scale_ctx,
                      vertical_ctx,
                      inrow_ofs_to_pointer (scale_ctx, ofs_y),
                      vertical_ctx->parts_row [0]);
    weight_row_128bpp (vertical_ctx->parts_row [0],
                       outrow_index == 0 ? 256 : 255 - scale_ctx->precalc_y [outrow_index * 2 - 1],
                       scale_ctx->width_out);
    ofs_y++;

    /* Add up whole rows */

    while (ofs_y < ofs_y_max)
    {
        scale_horizontal (scale_ctx,
                          vertical_ctx,
                          inrow_ofs_to_pointer (scale_ctx, ofs_y),
                          vertical_ctx->parts_row [1]);
        add_parts (vertical_ctx->parts_row [1],
                   vertical_ctx->parts_row [0],
                   scale_ctx->width_out * 2);

        ofs_y++;
    }

    /* Final row is optional; if this is the bottommost outrow it could be out of bounds */

    w = scale_ctx->precalc_y [outrow_index * 2 + 1];
    if (w > 0)
    {
        scale_horizontal (scale_ctx,
                          vertical_ctx,
                          inrow_ofs_to_pointer (scale_ctx, ofs_y),
                          vertical_ctx->parts_row [1]);
        weight_row_128bpp (vertical_ctx->parts_row [1],
                           w - 1,  /* Subtract 1 to avoid overflow */
                           scale_ctx->width_out);
        add_parts (vertical_ctx->parts_row [1],
                   vertical_ctx->parts_row [0],
                   scale_ctx->width_out * 2);
    }

    finalize_vertical_128bpp (vertical_ctx->parts_row [0],
                              scale_ctx->span_mul_y,
                              vertical_ctx->parts_row [1],
                              scale_ctx->width_out);
    scale_ctx->pack_row_func (vertical_ctx->parts_row [1], row_out, scale_ctx->width_out);
}

static void
scale_outrow_one_64bpp (const SmolScaleCtx *scale_ctx,
                        SmolVerticalCtx *vertical_ctx,
                        uint32_t row_index,
                        uint32_t *row_out)
{
    SMOL_UNUSED (row_index);

    /* Scale the row and store it */

    if (vertical_ctx->in_ofs != 0)
    {
        scale_horizontal (scale_ctx,
                          vertical_ctx,
                          inrow_ofs_to_pointer (scale_ctx, 0),
                          vertical_ctx->parts_row [0]);
        vertical_ctx->in_ofs = 0;
    }

    scale_ctx->pack_row_func (vertical_ctx->parts_row [0], row_out, scale_ctx->width_out);
}

static void
scale_outrow_one_128bpp (const SmolScaleCtx *scale_ctx,
                         SmolVerticalCtx *vertical_ctx,
                         uint32_t row_index,
                         uint32_t *row_out)
{
    SMOL_UNUSED (row_index);

    /* Scale the row and store it */

    if (vertical_ctx->in_ofs != 0)
    {
        scale_horizontal (scale_ctx,
                          vertical_ctx,
                          inrow_ofs_to_pointer (scale_ctx, 0),
                          vertical_ctx->parts_row [0]);
        vertical_ctx->in_ofs = 0;
    }

    scale_ctx->pack_row_func (vertical_ctx->parts_row [0], row_out, scale_ctx->width_out);
}

static void
scale_outrow_copy (const SmolScaleCtx *scale_ctx,
                   SmolVerticalCtx *vertical_ctx,
                   uint32_t row_index,
                   uint32_t *row_out)
{
    scale_horizontal (scale_ctx,
                      vertical_ctx,
                      inrow_ofs_to_pointer (scale_ctx, row_index),
                      vertical_ctx->parts_row [0]);

    scale_ctx->pack_row_func ((const void *)(vertical_ctx->parts_row [0]), (void *)row_out, scale_ctx->width_out);
}

/* --------------- *
 * Function tables *
 * --------------- */

#define R SMOL_REPACK_META

static const SmolRepackMeta repack_meta [] =
{
    R (123,   24, PREMUL8,      COMPRESSED, 1324,  64, PREMUL8,       COMPRESSED),

    R (123,   24, PREMUL8,      COMPRESSED, 1234, 128, PREMUL8,       COMPRESSED),
    R (123,   24, PREMUL8,      COMPRESSED, 1234, 128, PREMUL8,       LINEAR),

    R (1234,  32, PREMUL8,      COMPRESSED, 1324,  64, PREMUL8,       COMPRESSED),
    R (1234,  32, PREMUL8,      COMPRESSED, 2431,  64, PREMUL8,       COMPRESSED),
    R (1234,  32, PREMUL8,      COMPRESSED, 3241,  64, PREMUL8,       COMPRESSED),
    R (1234,  32, UNASSOCIATED, COMPRESSED, 1324,  64, PREMUL8,       COMPRESSED),
    R (1234,  32, UNASSOCIATED, COMPRESSED, 2431,  64, PREMUL8,       COMPRESSED),
    R (1234,  32, UNASSOCIATED, COMPRESSED, 3241,  64, PREMUL8,       COMPRESSED),

    R (1234,  32, PREMUL8,      COMPRESSED, 1234, 128, PREMUL8,       COMPRESSED),
    R (1234,  32, PREMUL8,      COMPRESSED, 2341, 128, PREMUL8,       COMPRESSED),
    R (1234,  32, UNASSOCIATED, COMPRESSED, 1234, 128, PREMUL8,       COMPRESSED),
    R (1234,  32, UNASSOCIATED, COMPRESSED, 2341, 128, PREMUL8,       COMPRESSED),
    R (1234,  32, UNASSOCIATED, COMPRESSED, 1234, 128, PREMUL16,      COMPRESSED),
    R (1234,  32, UNASSOCIATED, COMPRESSED, 2341, 128, PREMUL16,      COMPRESSED),
    R (1234,  32, PREMUL8,      COMPRESSED, 1234, 128, PREMUL8,       LINEAR),
    R (1234,  32, PREMUL8,      COMPRESSED, 2341, 128, PREMUL8,       LINEAR),
    R (1234,  32, UNASSOCIATED, COMPRESSED, 1234, 128, PREMUL8,       LINEAR),
    R (1234,  32, UNASSOCIATED, COMPRESSED, 2341, 128, PREMUL8,       LINEAR),
    R (1234,  32, UNASSOCIATED, COMPRESSED, 1234, 128, PREMUL16,      LINEAR),
    R (1234,  32, UNASSOCIATED, COMPRESSED, 2341, 128, PREMUL16,      LINEAR),

    R (1234,  64, PREMUL8,      COMPRESSED, 132,   24, PREMUL8,       COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 231,   24, PREMUL8,       COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 324,   24, PREMUL8,       COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 423,   24, PREMUL8,       COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 132,   24, UNASSOCIATED,  COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 231,   24, UNASSOCIATED,  COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 324,   24, UNASSOCIATED,  COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 423,   24, UNASSOCIATED,  COMPRESSED),

    R (1234,  64, PREMUL8,      COMPRESSED, 1324,  32, PREMUL8,       COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 1423,  32, PREMUL8,       COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 2314,  32, PREMUL8,       COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 4132,  32, PREMUL8,       COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 4231,  32, PREMUL8,       COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 1324,  32, UNASSOCIATED,  COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 1423,  32, UNASSOCIATED,  COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 2314,  32, UNASSOCIATED,  COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 4132,  32, UNASSOCIATED,  COMPRESSED),
    R (1234,  64, PREMUL8,      COMPRESSED, 4231,  32, UNASSOCIATED,  COMPRESSED),

    R (1234, 128, PREMUL8,      COMPRESSED, 123,   24, PREMUL8,       COMPRESSED),
    R (1234, 128, PREMUL8,      COMPRESSED, 321,   24, PREMUL8,       COMPRESSED),
    R (1234, 128, PREMUL8,      COMPRESSED, 123,   24, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL8,      COMPRESSED, 321,   24, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL16,     COMPRESSED, 123,   24, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL16,     COMPRESSED, 321,   24, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL8,      LINEAR, 123,   24, PREMUL8,       COMPRESSED),
    R (1234, 128, PREMUL8,      LINEAR, 321,   24, PREMUL8,       COMPRESSED),
    R (1234, 128, PREMUL8,      LINEAR, 123,   24, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL8,      LINEAR, 321,   24, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL16,     LINEAR, 123,   24, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL16,     LINEAR, 321,   24, UNASSOCIATED,  COMPRESSED),

    R (1234, 128, PREMUL8,      COMPRESSED, 1234,  32, PREMUL8,       COMPRESSED),
    R (1234, 128, PREMUL8,      COMPRESSED, 3214,  32, PREMUL8,       COMPRESSED),
    R (1234, 128, PREMUL8,      COMPRESSED, 4123,  32, PREMUL8,       COMPRESSED),
    R (1234, 128, PREMUL8,      COMPRESSED, 4321,  32, PREMUL8,       COMPRESSED),
    R (1234, 128, PREMUL8,      COMPRESSED, 1234,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL8,      COMPRESSED, 3214,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL8,      COMPRESSED, 4123,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL8,      COMPRESSED, 4321,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL16,     COMPRESSED, 1234,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL16,     COMPRESSED, 3214,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL16,     COMPRESSED, 4123,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL16,     COMPRESSED, 4321,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL8,      LINEAR, 1234,  32, PREMUL8,       COMPRESSED),
    R (1234, 128, PREMUL8,      LINEAR, 3214,  32, PREMUL8,       COMPRESSED),
    R (1234, 128, PREMUL8,      LINEAR, 4123,  32, PREMUL8,       COMPRESSED),
    R (1234, 128, PREMUL8,      LINEAR, 4321,  32, PREMUL8,       COMPRESSED),
    R (1234, 128, PREMUL8,      LINEAR, 1234,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL8,      LINEAR, 3214,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL8,      LINEAR, 4123,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL8,      LINEAR, 4321,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL16,     LINEAR, 1234,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL16,     LINEAR, 3214,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL16,     LINEAR, 4123,  32, UNASSOCIATED,  COMPRESSED),
    R (1234, 128, PREMUL16,     LINEAR, 4321,  32, UNASSOCIATED,  COMPRESSED),


    SMOL_REPACK_META_LAST
};

#undef R

static const SmolImplementation implementation =
{
    /* Horizontal init */
    init_horizontal,

    /* Vertical init */
    init_vertical,

    {
        /* Horizontal filters */
        {
            NULL /* 24bpp */
        },
        {
            NULL /* 32bpp */
        },
        {
            /* 64bpp */
            interp_horizontal_copy_64bpp,
            interp_horizontal_one_64bpp,
            interp_horizontal_bilinear_0h_64bpp,
            interp_horizontal_bilinear_1h_64bpp,
            interp_horizontal_bilinear_2h_64bpp,
            interp_horizontal_bilinear_3h_64bpp,
            interp_horizontal_bilinear_4h_64bpp,
            interp_horizontal_bilinear_5h_64bpp,
            interp_horizontal_bilinear_6h_64bpp,
            interp_horizontal_boxes_64bpp
        },
        {
            /* 128bpp */
            interp_horizontal_copy_128bpp,
            interp_horizontal_one_128bpp,
            interp_horizontal_bilinear_0h_128bpp,
            interp_horizontal_bilinear_1h_128bpp,
            interp_horizontal_bilinear_2h_128bpp,
            interp_horizontal_bilinear_3h_128bpp,
            interp_horizontal_bilinear_4h_128bpp,
            interp_horizontal_bilinear_5h_128bpp,
            interp_horizontal_bilinear_6h_128bpp,
            interp_horizontal_boxes_128bpp
        }
    },
    {
        /* Vertical filters */
        {
            NULL /* 24bpp */
        },
        {
            NULL /* 32bpp */
        },
        {
            /* 64bpp */
            scale_outrow_copy,
            scale_outrow_one_64bpp,
            scale_outrow_bilinear_0h_64bpp,
            scale_outrow_bilinear_1h_64bpp,
            scale_outrow_bilinear_2h_64bpp,
            scale_outrow_bilinear_3h_64bpp,
            scale_outrow_bilinear_4h_64bpp,
            scale_outrow_bilinear_5h_64bpp,
            scale_outrow_bilinear_6h_64bpp,
            scale_outrow_box_64bpp
        },
        {
            /* 128bpp */
            scale_outrow_copy,
            scale_outrow_one_128bpp,
            scale_outrow_bilinear_0h_128bpp,
            scale_outrow_bilinear_1h_128bpp,
            scale_outrow_bilinear_2h_128bpp,
            scale_outrow_bilinear_3h_128bpp,
            scale_outrow_bilinear_4h_128bpp,
            scale_outrow_bilinear_5h_128bpp,
            scale_outrow_bilinear_6h_128bpp,
            scale_outrow_box_128bpp
        }
    },
    repack_meta
};

const SmolImplementation *
_smol_get_generic_implementation (void)
{
    return &implementation;
}

#ifdef UNITTESTS

/* ---------- *
 * Unit tests *
 * ---------- */

/* To build and run:
 *
 * gcc -DUNITTESTS smolscale.c smolscale-generic.c -o generic-tests
 * ./generic-tests | grep Err
 */

#include <stdio.h>

static void
unpack_128bpp (const uint64_t *p, uint16_t *t)
{
    t [0] = p [0] >> 32;
    t [1] = p [0] & 0x00000000ffffffffULL;
    t [2] = p [1] >> 32;
    t [3] = p [1] & 0x00000000ffffffffULL;
}

static void
unpack_64bpp (uint64_t p, uint16_t *t)
{
    t [0] = p & 0xff;
    t [1] = (p >> 16) & 0xff;
    t [2] = (p >> 32) & 0xff;
    t [3] = (p >> 48) & 0xff;
}

static int
cmp_channels_8bit (int a, int b, int alpha)
{
    if (alpha == 0 && b == 0)
        return 0;

    if ((abs (a - b) > 0)
        || a > 0xff || b > 0xff)
        return 1;

    return 0;
}

static int
cmp_channels_8bit_fuzzy (int a, int b, int alpha)
{
    if (alpha == 0 && b == 0)
        return 0;

    if ((alpha > 16 && abs (a - b) > 15)
        || a > 0xff || b > 0xff)
        return 1;

    return 0;
}

static int
cmp_channels_11bit (int a, int b, int alpha)
{
    if (alpha == 0 && b == 0)
        return 0;

    if ((abs (a - b) > 0)
        || a > 0x7ff || b > 0x7ff)
        return 1;

    return 0;
}

static int
cmp_channels_11bit_fuzzy (int a, int b, int alpha)
{
    if (alpha == 0 && b == 0)
        return 0;

    if ((alpha > 16 && abs (a - b) > 15)
        || a > 0x7ff || b > 0x7ff)
        return 1;

    return 0;
}

static void
test_p8 (void)
{
    uint64_t i;

    for (i = 0; i < 0x100; i++)
    {
        uint64_t p [3] [2];
        uint16_t t [3] [4];
        unsigned int alpha;

        p [0] [0] = (i << 32) | i;
        p [0] [1] = (i << 32);

        for (alpha = 0; alpha < 256; alpha++)
        {
            int is_good;

            memcpy (p [1], p [0], sizeof (uint64_t) * 2);
            premul_u_to_p8_128bpp (p [1], alpha);
            unpremul_p8_to_u_128bpp (p [1], p [2], alpha);

            unpack_128bpp (p [0], t [0]);
            unpack_128bpp (p [1], t [1]);
            unpack_128bpp (p [2], t [2]);

            is_good = !(cmp_channels_8bit_fuzzy (t [0] [0], t [2] [0], alpha)
                        | cmp_channels_8bit_fuzzy (t [0] [1], t [2] [1], alpha)
                        | cmp_channels_8bit_fuzzy (t [0] [2], t [2] [2], alpha)
                        | cmp_channels_8bit_fuzzy (t [0] [3], t [2] [3], alpha));

            printf ("%s   %016lx/%016lx -> %016lx/%016lx\n"
                    "     -> %016lx/%016lx (a=%02x)\n\n",
                    is_good ? "     " : "Err: ",
                    p [0] [0], p [0] [1],
                    p [1] [0], p [1] [1],
                    p [2] [0], p [2] [1],
                    alpha);
        }
    }
}

static void
test_p8_64bpp (void)
{
    uint64_t i;

    for (i = 0; i < 0x100; i++)
    {
        uint64_t p [3];
        uint16_t t [3] [4];
        unsigned int alpha;

        p [0] = (i << 48) | (i << 32) | (i << 16);

        for (alpha = 0; alpha < 256; alpha++)
        {
            int is_good;

            p [1] = premul_u_to_p8_64bpp (p [0], alpha);
            p [2] = unpremul_p8_to_u_64bpp (p [1], alpha);

            unpack_64bpp (p [0], t [0]);
            unpack_64bpp (p [1], t [1]);
            unpack_64bpp (p [2], t [2]);

            is_good = !(cmp_channels_8bit_fuzzy (t [0] [0], t [2] [0], alpha)
                        | cmp_channels_8bit_fuzzy (t [0] [1], t [2] [1], alpha)
                        | cmp_channels_8bit_fuzzy (t [0] [2], t [2] [2], alpha)
                        | cmp_channels_8bit_fuzzy (t [0] [3], t [2] [3], alpha));

            printf ("%s   %016lx -> %016lx\n"
                    "     -> %016lx (a=%02x)\n\n",
                    is_good ? "     " : "Err: ",
                    p [0],
                    p [1],
                    p [2],
                    alpha);
        }
    }
}
static void
test_p8l (void)
{
    uint64_t i;

    for (i = 0; i < 0x800; i++)
    {
        uint64_t p [3] [2];
        uint16_t t [3] [4];
        unsigned int alpha;

        p [0] [0] = (i << 32) | i;
        p [0] [1] = (i << 32);

        for (alpha = 0; alpha < 256; alpha++)
        {
            int is_good;

            memcpy (p [1], p [0], sizeof (uint64_t) * 2);
            premul_ul_to_p8l_128bpp (p [1], alpha);
            unpremul_p8l_to_ul_128bpp (p [1], p [2], alpha);

            unpack_128bpp (p [0], t [0]);
            unpack_128bpp (p [1], t [1]);
            unpack_128bpp (p [2], t [2]);

            is_good = !(cmp_channels_11bit_fuzzy (t [0] [0], t [2] [0], alpha)
                        | cmp_channels_11bit_fuzzy (t [0] [1], t [2] [1], alpha)
                        | cmp_channels_11bit_fuzzy (t [0] [2], t [2] [2], alpha)
                        | cmp_channels_11bit_fuzzy (t [0] [3], t [2] [3], alpha));

            printf ("%s   %016lx/%016lx -> %016lx/%016lx\n"
                    "     -> %016lx/%016lx (a=%02x)\n\n",
                    is_good ? "     " : "Err: ",
                    p [0] [0], p [0] [1],
                    p [1] [0], p [1] [1],
                    p [2] [0], p [2] [1],
                    alpha);
        }
    }
}

static void
test_p16 (void)
{
    uint64_t i;

    for (i = 0; i < 0x100; i++)
    {
        uint64_t p [3] [2];
        uint16_t t [3] [4];
        unsigned int alpha;

        p [0] [0] = (i << 32) | i;
        p [0] [1] = (i << 32);

        for (alpha = 0; alpha < 256; alpha++)
        {
            int is_good;

            memcpy (p [1], p [0], sizeof (uint64_t) * 2);

            premul_u_to_p16_128bpp (p [1], alpha);
            p [1] [0] &= 0x0000ffff0000ffffULL;
            p [1] [1] &= 0x0000ffff00000000ULL;

            unpremul_p16_to_u_128bpp (p [1], p [2], alpha);
            p [2] [0] &= 0x000000ff000000ffULL;
            p [2] [1] &= 0x000000ff00000000ULL;

            unpack_128bpp (p [0], t [0]);
            unpack_128bpp (p [1], t [1]);
            unpack_128bpp (p [2], t [2]);

            is_good = !(cmp_channels_8bit (t [0] [0], t [2] [0], alpha)
                        | cmp_channels_8bit (t [0] [1], t [2] [1], alpha)
                        | cmp_channels_8bit (t [0] [2], t [2] [2], alpha)
                        | cmp_channels_8bit (t [0] [3], t [2] [3], alpha));

            printf ("%s   %016lx/%016lx -> %016lx/%016lx\n"
                    "     -> %016lx/%016lx (a=%02x)\n\n",
                    is_good ? "     " : "Err: ",
                    p [0] [0], p [0] [1],
                    p [1] [0], p [1] [1],
                    p [2] [0], p [2] [1],
                    alpha);
        }
    }
}

static void
test_p16l (void)
{
    uint64_t i;

    for (i = 0; i < 0x800; i++)
    {
        uint64_t p [3] [2];
        uint16_t t [3] [4];
        unsigned int alpha;

        p [0] [0] = (i << 32) | i;
        p [0] [1] = (i << 32);

        for (alpha = 0; alpha < 256; alpha++)
        {
            int is_good;

            memcpy (p [1], p [0], sizeof (uint64_t) * 2);

            premul_ul_to_p16l_128bpp (p [1], alpha);
            p [1] [0] &= 0x0007ffff0007ffffULL;
            p [1] [1] &= 0x0007ffff00000000ULL;

            unpremul_p16l_to_ul_128bpp (p [1], p [2], alpha);
            p [2] [0] &= 0x000007ff000007ffULL;
            p [2] [1] &= 0x000007ff00000000ULL;

            unpack_128bpp (p [0], t [0]);
            unpack_128bpp (p [1], t [1]);
            unpack_128bpp (p [2], t [2]);

            is_good = !(cmp_channels_11bit (t [0] [0], t [2] [0], alpha)
                        | cmp_channels_11bit (t [0] [1], t [2] [1], alpha)
                        | cmp_channels_11bit (t [0] [2], t [2] [2], alpha)
                        | cmp_channels_11bit (t [0] [3], t [2] [3], alpha));

            printf ("%s   %016lx/%016lx -> %016lx/%016lx\n"
                    "     -> %016lx/%016lx (a=%02x)\n\n",
                    is_good ? "     " : "Err: ",
                    p [0] [0], p [0] [1],
                    p [1] [0], p [1] [1],
                    p [2] [0], p [2] [1],
                    alpha);
        }
    }
}

static void
test_srgb (void)
{
    uint64_t i;

    for (i = 0; i < 256; i++)
    {
        uint64_t p [3] [2];
        uint16_t t [3] [4];

        p [0] [0] = (i << 32) | i;
        p [0] [1] = (i << 32) | 0xff;

        memcpy (p [1], p [0], sizeof (uint64_t) * 2);
        from_srgb_pixel_xxxa_128bpp (p [1]);
        to_srgb_pixel_xxxa_128bpp (p [1], p [2]);

        if (memcmp (p [0], p [2], sizeof (uint64_t) * 2))
        {
            printf ("sRGB revert failed: %016lx/%016lx -> %016lx/%016lx\n"
                    "                    -> %016lx/%016lx\n",
                    p [0] [0], p [0] [1],
                    p [1] [0], p [1] [1],
                    p [2] [0], p [2] [1]);
        }
    }
}

static void
test_p8_to_p8_with_srgb (void)
{
    uint64_t i;
    uint64_t alpha;

    for (alpha = 1; alpha < 256; alpha++)
    {
        for (i = 0; i <= alpha; i++)
        {
            uint64_t p [7] [2];
            uint16_t t [2] [4];
            int is_good;

            p [0] [0] = (i << 32) | i;
            p [0] [1] = (i << 32) | alpha;

            unpremul_p8_to_u_128bpp (p [0], p [1], alpha);
            memcpy (p [2], p [1], sizeof (uint64_t) * 2);
            from_srgb_pixel_xxxa_128bpp (p [2]);
            memcpy (p [3], p [2], sizeof (uint64_t) * 2);
            premul_ul_to_p8l_128bpp (p [3], alpha);

            unpremul_p8l_to_ul_128bpp (p [3], p [4], alpha);
            to_srgb_pixel_xxxa_128bpp (p [4], p [5]);
            memcpy (p [6], p [5], sizeof (uint64_t) * 2);
            premul_u_to_p8_128bpp (p [6], alpha);

            p [6] [1] = (p [6] [1] & 0xffffffff00000000) | alpha;

            unpack_128bpp (p [0], t [0]);
            unpack_128bpp (p [6], t [1]);

            is_good = !(cmp_channels_8bit_fuzzy (t [0] [0], t [1] [0], alpha)
                        | cmp_channels_8bit_fuzzy (t [0] [1], t [1] [1], alpha)
                        | cmp_channels_8bit_fuzzy (t [0] [2], t [1] [2], alpha)
                        | cmp_channels_8bit_fuzzy (t [0] [3], t [1] [3], alpha));

            if (!is_good)
            {
                printf ("p->l->p failed:   %016lx/%016lx -u-> %016lx/%016lx\n"
                        "             -L-> %016lx/%016lx -p-> %016lx/%016lx\n"
                        "             -u-> %016lx/%016lx -l-> %016lx/%016lx\n"
                        "             -p-> %016lx/%016lx\n",
                        p [0] [0], p [0] [1],
                        p [1] [0], p [1] [1],
                        p [2] [0], p [2] [1],
                        p [3] [0], p [3] [1],
                        p [4] [0], p [4] [1],
                        p [5] [0], p [5] [1],
                        p [6] [0], p [6] [1]);
            }
        }
    }
}

int
main (int argc, char *argv [])
{
    test_p8_64bpp ();
    test_p8 ();
    test_p8l ();
    test_p16 ();
    test_p16l ();
    test_srgb ();
    test_p8_to_p8_with_srgb ();
}

#endif
