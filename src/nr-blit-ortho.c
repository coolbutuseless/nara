
#define R_NO_REMAP

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "color.h"
#include "nr-core.h"
#include "nr-utils.h"
#include "nr-draw.h"
#include "nr-blit-ortho.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprite onto raster [C interface]
//
// This method only blits image with rotation or zooming i.e. scale = 1, theta = 0
// 
// Function tries to be smart
//  - trim coordinates to ensure they lie within the src/dst images
//
// @param dst destination native raster
// @param dst_width,dst_height dimensions of destination
// @param x,y position within destination image
// @param src source native raster
// @param src_width,src_height dimensions of source
// @param xsrc,ysrc starting position within source
// @param w,h dimensions of region to copy
// @param respect_alpha Should alpha values be respected when blitting?
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_blit_ortho(uint32_t *dst, int dst_width, int dst_height, int x   , int y , 
                   uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, 
                   int w, int h,
                   double hjust, double vjust,
                   bool respect_alpha) {
  
  
  
  int hjust_delta = (int)round(hjust * (w - 1));
  int vjust_delta = (int)round(vjust * (h - 1));
  
  x -= hjust_delta;
  y -= vjust_delta;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Check if src doesn't overlap with dst **AT ALL**
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (
      x + w < 0 ||
        y + h < 0 ||
        x >= dst_width ||
        y >= dst_height
  ) {
    return;
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Some of the src overlaps the dst
  // Trim src boundaries if they're outside the image
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (x < 0) {
    // Trim left of src
    int diff = -x;
    xsrc += diff;
    w    -= diff ;
    x     = 0;
  }
  if (y < 0) {
    // Trim bottom of src
    int diff = -y ;
    ysrc += diff;
    h    -= diff ;
    y     = 0;
  }
  if (x + w >= dst_width) {
    // Trim right of src
    w = dst_width - x;
  }
  if (y + h >= dst_height) {
    // Trim right of src
    h = dst_height - y;
  }
  
  if (w == 0 || h == 0) return;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If 'respect_alpha', then copy pixel by pixel
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (respect_alpha) {
    for (int yoff = 0; yoff < h; yoff++) {
      int y1 = ysrc + yoff ;
      for (int xoff = 0; xoff < w; xoff++) {
        uint32_t src_col = src[y1 * src_width + xsrc + xoff];
        nr_point(dst, dst_width, dst_height, x + xoff, y + yoff, src_col);
      }
    }
  } else {
    // Blit it via 'memcpy()' trashing any contents
    // Flip y axis. matrices are (0, 0) at top left,
    for (int yoff = 0; yoff < h; yoff++) {
      int y1 = ysrc + yoff;
      memcpy(
        dst + (y + yoff) * dst_width + x, 
        src + y1 * src_width + xsrc, 
        (unsigned long)w * sizeof(int32_t)
      );
    }
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprite onto raster [C interface]
// Slow and study, but it works.
// It attempts to plot every single point regardless of whether it
// falls inside the boundary or not
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_blit_ortho_naive(uint32_t *dst, int dst_width, int dst_height, int x, int y, 
                      uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, 
                      int w, int h) {
  
  for (int yoff = 0; yoff < h; yoff++) {
    int y1 = src_height - ysrc - yoff;
    for (int xoff = 0; xoff < w; xoff++) {
      
      uint32_t src_col = src[y1 * src_width + xsrc + xoff - 1];
      
      nr_point(dst, dst_width, dst_height, x + xoff, y + yoff, src_col);
    }
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprites into raster [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_blit_ortho_(SEXP dst_, SEXP x_    , SEXP y_,
                    SEXP src_, SEXP xsrc_   , SEXP ysrc_, 
                    SEXP w_    , SEXP h_,
                    SEXP hjust_, SEXP vjust_, 
                    SEXP respect_alpha_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity checks
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(dst_);
  assert_nativeraster(src_);
  
  if (!(Rf_length(x_) == 1 || Rf_length(y_) == 1 || Rf_length(x_) == Rf_length(y_))) {
    Rf_error("'x' and 'y' must be the length 1 or N.  x = %i, y = %i", Rf_length(x_), Rf_length(y_));
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Dimensions
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int dst_width  = Rf_ncols(dst_);
  int dst_height = Rf_nrows(dst_);
  int src_width  = Rf_ncols(src_);
  int src_height = Rf_nrows(src_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Ensure the coordinates are integers
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool freex = false, freey = false;
  int N = calc_max_length(2, x_, y_);
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Single native raster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int xsrc = Rf_asInteger(xsrc_);
  int ysrc = Rf_asInteger(ysrc_);
  
  
  int w = Rf_asInteger(w_) < 0 ? src_width  : Rf_asInteger(w_);
  int h = Rf_asInteger(h_) < 0 ? src_height : Rf_asInteger(h_);
  
  if (xsrc + w - 1 >= src_width || ysrc + h - 1 >= src_height) {
    Rf_error("Specified src [xsrc = %i, ysrc = %i] + [w = %i, h = %i] is outside bounds [%i, %i]",
          xsrc, ysrc, w, h, src_width, src_height);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Blit mulitple copies
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint32_t *dst = (uint32_t *)INTEGER(dst_);
  uint32_t *src = (uint32_t *)INTEGER(src_);
  bool respect_alpha = Rf_asLogical(respect_alpha_);
  for (int i = 0; i < Rf_length(x_); i++) {
    nr_blit_ortho(
      dst, dst_width, dst_height, x[i], y[i], 
      src, src_width, src_height, xsrc, ysrc, 
      w, h, 
      Rf_asReal(hjust_), Rf_asReal(vjust_),
      respect_alpha);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Tidy and return
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (freex) free(x);
  if (freey) free(y);
  return dst_;
}


