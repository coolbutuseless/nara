/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/* Copyright © 2019-2023 Hans Petter Jansson. See COPYING for details. */

/* If you're just going to use Smolscale in your project, you don't have to
 * worry about anything in here. The public API and documentation, such as
 * it is, lives in smolscale.h.
 *
 * If, on the other hand, you're here to hack on Smolscale itself, this file
 * contains all the internal shared declarations. */

#include <R.h>
#include <stdint.h>
#include "smolscale.h"

#ifndef _SMOLSCALE_PRIVATE_H_
#define _SMOLSCALE_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

// #ifdef SMOL_USE_ALLOCA
// # define _SMOL_ALLOC(n) alloca (n)
// # define _SMOL_FREE(p)
// #else
// # define _SMOL_ALLOC(n) malloc (n)
// # define _SMOL_FREE(p) free (p)
// #endif

// R's R_Calloc is 8-byte aligned
static inline void *R_calloc2(size_t n){
  char *tmp = R_Calloc(n, char); // 64 bit aligned
  memset(tmp, 0, n);
  return (void*) tmp;
}
  
# define _SMOL_ALLOC(n) R_calloc2 (n)
# define _SMOL_FREE(p) R_Free (p)
  
/* Enum switches must handle every value */
#ifdef __GNUC__
# pragma GCC diagnostic error "-Wswitch"
#endif

/* Compensate for GCC missing intrinsics */
#ifdef __GNUC__
# if __GNUC__ < 8
#  define _mm256_set_m128i(h, l) \
    _mm256_insertf128_si256 (_mm256_castsi128_si256 (l), (h), 1)
# endif
#endif

#ifndef FALSE
# define FALSE (0)
#endif
#ifndef TRUE
# define TRUE (!FALSE)
#endif
#ifndef MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

typedef unsigned int SmolBool;

#define SMOL_4X2BIT(a, b, c, d)                                         \
    (((a) << 6) | ((b) << 4) | ((c) << 2) | (d))

#define SMOL_8X1BIT(a,b,c,d,e,f,g,h)                                    \
    (((a) << 7) | ((b) << 6) | ((c) << 5) | ((d) << 4)                  \
     | ((e) << 3) | ((f) << 2) | ((g) << 1) | ((h) << 0))

#define SMOL_UNUSED(x) (void) ((x)=(x))
#define SMOL_RESTRICT __restrict
#define SMOL_INLINE __attribute__((always_inline)) inline
#define SMOL_CONST __attribute__((const))
#define SMOL_PURE __attribute__((pure))

#define SMOL_SMALL_MUL 256U
#define SMOL_BIG_MUL 65536U
#define SMOL_BOXES_MULTIPLIER ((uint64_t) SMOL_BIG_MUL * SMOL_SMALL_MUL)
#define SMOL_BILIN_MULTIPLIER ((uint64_t) SMOL_BIG_MUL * SMOL_BIG_MUL)

#define SMOL_ALIGNMENT 64

#define SMOL_ASSIGN_ALIGNED_TO(x, t, n) (t) __builtin_assume_aligned ((x), (n))
#define SMOL_ASSIGN_ALIGNED(x, t) SMOL_ASSIGN_ALIGNED_TO ((x), t, SMOL_ALIGNMENT)

#define SMOL_ASSUME_ALIGNED_TO(x, t, n) (x) = SMOL_ASSIGN_ALIGNED_TO ((x), t, (n))
#define SMOL_ASSUME_ALIGNED(x, t) SMOL_ASSUME_ALIGNED_TO ((x), t, SMOL_ALIGNMENT)

/* Pointer to beginning of storage is stored in *r. This must be passed to smol_free() later. */
#define smol_alloc_aligned_to(s, a, r) \
  ({ void *p; *(r) = _SMOL_ALLOC ((s) + (a)); p = (void *) (((uintptr_t) (*(r)) + (a)) & ~((a) - 1)); (p); })
// #define smol_alloc_aligned(s, r) smol_alloc_aligned_to ((s), SMOL_ALIGNMENT, (r))
#define smol_alloc_aligned(s, r) R_calloc2((s))

#define smol_free(p) _SMOL_FREE(p)

typedef enum
{
    SMOL_STORAGE_24BPP,
    SMOL_STORAGE_32BPP,
    SMOL_STORAGE_64BPP,
    SMOL_STORAGE_128BPP,

    SMOL_STORAGE_MAX
}
SmolStorageType;

typedef enum
{
    SMOL_FILTER_COPY,
    SMOL_FILTER_ONE,
    SMOL_FILTER_BILINEAR_0H,
    SMOL_FILTER_BILINEAR_1H,
    SMOL_FILTER_BILINEAR_2H,
    SMOL_FILTER_BILINEAR_3H,
    SMOL_FILTER_BILINEAR_4H,
    SMOL_FILTER_BILINEAR_5H,
    SMOL_FILTER_BILINEAR_6H,
    SMOL_FILTER_BOX,

    SMOL_FILTER_MAX
}
SmolFilterType;

typedef enum
{
    SMOL_REORDER_1234_TO_1234,

    SMOL_REORDER_1234_TO_2341,
    SMOL_REORDER_1234_TO_3214,
    SMOL_REORDER_1234_TO_4123,
    SMOL_REORDER_1234_TO_4321,
    SMOL_REORDER_1234_TO_123,
    SMOL_REORDER_1234_TO_321,
    SMOL_REORDER_123_TO_1234,

    SMOL_REORDER_1234_TO_1324,
    SMOL_REORDER_1234_TO_2314,
    SMOL_REORDER_1234_TO_2431,
    SMOL_REORDER_1234_TO_4132,
    SMOL_REORDER_1234_TO_4231,
    SMOL_REORDER_1234_TO_132,
    SMOL_REORDER_1234_TO_231,
    SMOL_REORDER_123_TO_1324,

    SMOL_REORDER_1234_TO_324,
    SMOL_REORDER_1234_TO_423,

    SMOL_REORDER_1234_TO_1423,
    SMOL_REORDER_1234_TO_3241,

    SMOL_REORDER_MAX
}
SmolReorderType;

typedef enum
{
    SMOL_ALPHA_UNASSOCIATED,
    SMOL_ALPHA_PREMUL8,
    SMOL_ALPHA_PREMUL16,

    SMOL_ALPHA_MAX
}
SmolAlphaType;

typedef enum
{
    SMOL_GAMMA_SRGB_COMPRESSED,
    SMOL_GAMMA_SRGB_LINEAR,

    SMOL_GAMMA_MAX
}
SmolGammaType;

typedef struct
{
    unsigned char src [4];
    unsigned char dest [4];
}
SmolReorderMeta;

typedef struct
{
    unsigned char storage;
    unsigned char alpha;
    unsigned char order [4];
}
SmolPixelTypeMeta;

/* For reusing rows that have already undergone horizontal scaling */
typedef struct
{
    uint32_t in_ofs;
    uint64_t *parts_row [4];
    uint64_t *row_storage [4];
    uint32_t *in_aligned;
    uint32_t *in_aligned_storage;
}
SmolVerticalCtx;

typedef void (SmolInitFunc) (SmolScaleCtx *scale_ctx);
typedef void (SmolUnpackRowFunc) (const uint32_t *row_in,
                                  uint64_t *row_out,
                                  uint32_t n_pixels);
typedef void (SmolPackRowFunc) (const uint64_t *row_in,
                                uint32_t *row_out,
                                uint32_t n_pixels);
typedef void (SmolRepackRowFunc) (const void *row_in,
                                  void *row_out,
                                  uint32_t n_pixels);
typedef void (SmolHFilterFunc) (const SmolScaleCtx *scale_ctx,
                                const uint64_t *row_limbs_in,
                                uint64_t *row_limbs_out);
typedef void (SmolVFilterFunc) (const SmolScaleCtx *scale_ctx,
                                SmolVerticalCtx *vertical_ctx,
                                uint32_t outrow_index,
                                uint32_t *row_out);

#define SMOL_REPACK_SIGNATURE_GET_REORDER(sig) ((sig) >> (2 * (SMOL_GAMMA_BITS + SMOL_ALPHA_BITS + SMOL_STORAGE_BITS)))

#define SMOL_REORDER_BITS 6
#define SMOL_STORAGE_BITS 2
#define SMOL_ALPHA_BITS 2
#define SMOL_GAMMA_BITS 1

#define SMOL_MAKE_REPACK_SIGNATURE_ANY_ORDER(storage_in, alpha_in, gamma_in, \
                                             storage_out, alpha_out, gamma_out) \
    (((storage_in) << (SMOL_GAMMA_BITS + SMOL_ALPHA_BITS + SMOL_STORAGE_BITS + SMOL_GAMMA_BITS + SMOL_ALPHA_BITS)) \
     | ((alpha_in) << (SMOL_GAMMA_BITS + SMOL_ALPHA_BITS + SMOL_STORAGE_BITS + SMOL_GAMMA_BITS)) \
     | ((gamma_in) << (SMOL_GAMMA_BITS + SMOL_ALPHA_BITS + SMOL_STORAGE_BITS)) \
     | ((storage_out) << (SMOL_GAMMA_BITS + SMOL_ALPHA_BITS))           \
     | ((alpha_out) << (SMOL_GAMMA_BITS))                               \
     | ((gamma_out) << 0))                                              \

#define MASK_ITEM(m, n_bits) ((m) ? (1 << (n_bits)) - 1 : 0)

#define SMOL_REPACK_SIGNATURE_ANY_ORDER_MASK(storage_in, alpha_in, gamma_in, \
                                             storage_out, alpha_out, gamma_out) \
    SMOL_MAKE_REPACK_SIGNATURE_ANY_ORDER(MASK_ITEM (storage_in, SMOL_STORAGE_BITS), \
                                         MASK_ITEM (alpha_in, SMOL_ALPHA_BITS), \
                                         MASK_ITEM (gamma_in, SMOL_GAMMA_BITS), \
                                         MASK_ITEM (storage_out, SMOL_STORAGE_BITS), \
                                         MASK_ITEM (alpha_out, SMOL_ALPHA_BITS), \
                                         MASK_ITEM (gamma_out, SMOL_GAMMA_BITS))

#define SMOL_REPACK_META(order_in, storage_in, alpha_in, gamma_in,      \
                         order_out, storage_out, alpha_out, gamma_out)  \
    { (((SMOL_REORDER_##order_in##_TO_##order_out) << 10)               \
       | ((SMOL_STORAGE_##storage_in##BPP) << 8) | ((SMOL_ALPHA_##alpha_in) << 6) \
       | ((SMOL_GAMMA_SRGB_##gamma_in) << 5)                            \
       | ((SMOL_STORAGE_##storage_out##BPP) << 3) | ((SMOL_ALPHA_##alpha_out) << 1) \
       | ((SMOL_GAMMA_SRGB_##gamma_out) << 0)), \
    (SmolRepackRowFunc *) repack_row_##order_in##_##storage_in##_##alpha_in##_##gamma_in##_to_##order_out##_##storage_out##_##alpha_out##_##gamma_out }

#define SMOL_REPACK_META_LAST { 0xffff, NULL }

typedef struct
{
    uint16_t signature;
    SmolRepackRowFunc *repack_row_func;
}
SmolRepackMeta;

#define SMOL_REPACK_ROW_DEF(order_in, storage_in, limb_bits_in, alpha_in, gamma_in, \
                            order_out, storage_out, limb_bits_out, alpha_out, gamma_out) \
    static void repack_row_##order_in##_##storage_in##_##alpha_in##_##gamma_in##_to_##order_out##_##storage_out##_##alpha_out##_##gamma_out \
    (const uint##limb_bits_in##_t * SMOL_RESTRICT row_in,               \
     uint##limb_bits_out##_t * SMOL_RESTRICT row_out,                   \
     uint32_t n_pixels)                                                 \
    {                                                                   \
        uint##limb_bits_out##_t *row_out_max = row_out + n_pixels * (storage_out / limb_bits_out); \
        SMOL_ASSUME_ALIGNED_TO (row_in, uint##limb_bits_in##_t *, limb_bits_in / 8); \
        SMOL_ASSUME_ALIGNED_TO (row_out, uint##limb_bits_out##_t *, limb_bits_out / 8);

#define SMOL_REPACK_ROW_DEF_END }

typedef struct
{
    SmolInitFunc *init_h_func;
    SmolInitFunc *init_v_func;
    SmolHFilterFunc *hfilter_funcs [SMOL_STORAGE_MAX] [SMOL_FILTER_MAX];
    SmolVFilterFunc *vfilter_funcs [SMOL_STORAGE_MAX] [SMOL_FILTER_MAX];
    const SmolRepackMeta *repack_meta;
}
SmolImplementation;

struct SmolScaleCtx
{
    /* <private> */

    const char *pixels_in;
    char *pixels_out;
    uint32_t width_in, height_in, rowstride_in;
    uint32_t width_out, height_out, rowstride_out;

    SmolPixelType pixel_type_in, pixel_type_out;
    SmolFilterType filter_h, filter_v;
    SmolStorageType storage_type;
    SmolGammaType gamma_type;

    SmolRepackRowFunc *unpack_row_func;
    SmolRepackRowFunc *pack_row_func;
    SmolHFilterFunc *hfilter_func;
    SmolVFilterFunc *vfilter_func;

    /* User specified, can be NULL */
    SmolPostRowFunc *post_row_func;
    void *user_data;

    /* Each offset is split in two uint16s: { pixel index, fraction }. These
     * are relative to the image after halvings have taken place. */
    uint16_t *precalc_x, *precalc_y;
    uint32_t span_mul_x, span_mul_y;  /* For box filter */

    void *precalc_x_storage;

    uint32_t width_bilin_out, height_bilin_out;
    unsigned int width_halvings, height_halvings;
};

#define SRGB_LINEAR_BITS 11
#define SRGB_LINEAR_MAX (1 << (SRGB_LINEAR_BITS))

extern const uint16_t _smol_from_srgb_lut [256];
extern const uint8_t _smol_to_srgb_lut [SRGB_LINEAR_MAX];

#define INVERTED_DIV_SHIFT_P8 (21 - 8)
#define INVERTED_DIV_SHIFT_P8L (21 - SRGB_LINEAR_BITS)
#define INVERTED_DIV_SHIFT_P16 (24 - 8)
#define INVERTED_DIV_SHIFT_P16L (30 - SRGB_LINEAR_BITS)

extern const uint32_t _smol_inv_div_p8_lut [256];
extern const uint32_t _smol_inv_div_p8l_lut [256];
extern const uint32_t _smol_inv_div_p16_lut [256];
extern const uint32_t _smol_inv_div_p16l_lut [256];

const SmolImplementation *_smol_get_generic_implementation (void);
#ifdef SMOL_WITH_AVX2
const SmolImplementation *_smol_get_avx2_implementation (void);
#endif

#ifdef __cplusplus
}
#endif

#endif
