


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "color.h"
#include "nr-utils.h"
#include "nr-draw.h"

#include "smolscale.h"
#include "smolscale-private.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize a native raster using 'smolscale' C library
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP resize_(SEXP nr_, SEXP out_width_, SEXP out_height_) {

  assert_nativeraster(nr_);
  
  // Get dims of src
  int  width  = Rf_ncols(nr_);
  int  height = Rf_nrows(nr_);
  
  int out_width  = asInteger(out_width_);
  int out_height = asInteger(out_height_);
  
  // Create nativeraster and copy contents
  SEXP dst_ = PROTECT(allocVector(INTSXP, out_width * out_height));
  SET_CLASS(dst_, mkString("nativeRaster"));
  SEXP nr_dim = PROTECT(allocVector(INTSXP, 2));
  INTEGER(nr_dim)[0] = out_height;
  INTEGER(nr_dim)[1] = out_width;
  SET_DIM(dst_, nr_dim);
  UNPROTECT(1);
  
  // typedef enum
  // {
  //   /* 32 bits per pixel */
  //   
  //   SMOL_PIXEL_RGBA8_PREMULTIPLIED,
  //   SMOL_PIXEL_BGRA8_PREMULTIPLIED,
  //   SMOL_PIXEL_ARGB8_PREMULTIPLIED,
  //   SMOL_PIXEL_ABGR8_PREMULTIPLIED,
  //   
  //   SMOL_PIXEL_RGBA8_UNASSOCIATED,
  //   SMOL_PIXEL_BGRA8_UNASSOCIATED,
  //   SMOL_PIXEL_ARGB8_UNASSOCIATED,
  //   SMOL_PIXEL_ABGR8_UNASSOCIATED,
  //   
  //   /* 24 bits per pixel */
  //   
  //   SMOL_PIXEL_RGB8,
  //   SMOL_PIXEL_BGR8,
  //   
  //   SMOL_PIXEL_MAX
  // }
  // SmolPixelType;
  
  // void smol_scale_simple (const void *pixels_in, SmolPixelType pixel_type_in,
  //                         uint32_t width_in, uint32_t height_in, uint32_t rowstride_in,
  //                         void *pixels_out, SmolPixelType pixel_type_out,
  //                         uint32_t width_out, uint32_t height_out, uint32_t rowstride_out,
  //                         uint8_t with_srgb);
  
  smol_scale_simple(
    (const void *)INTEGER(nr_),
    SMOL_PIXEL_RGBA8_UNASSOCIATED,
    (uint32_t)width,
    (uint32_t)height,
    (uint32_t)width * 4,
    (void *)INTEGER(dst_),
    SMOL_PIXEL_RGBA8_UNASSOCIATED,
    (uint32_t)out_width,
    (uint32_t)out_height,
    (uint32_t)out_width * 4,
    SMOL_GAMMA_SRGB_COMPRESSED
    // SMOL_GAMMA_SRGB_LINEAR
  );
  
  
  UNPROTECT(1);
  return dst_;
}
