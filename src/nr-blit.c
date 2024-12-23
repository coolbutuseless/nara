
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
#include "nr-utils.h"
#include "nr-draw.h"



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprite onto raster [C interface]
// 
// Function tries to be smart
//  - trim coordinates to ensure they lie within the src/dst images
//
// @param dst destination native raster
// @param dst_width,dst_height dimensions of destination
// @param x,y position within destination image
// @param src source native raster
// @param src_width,src_height dimensions of source
// @param x0,y0 starting position within source
// @param w,h dimensions of region to copy
// @param respect_alpha Should alpha values be respected when blitting?
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void blit_core_(uint32_t *dst, int dst_width, int dst_height, int x , int y , 
                uint32_t *src, int src_width, int src_height, int x0, int y0, 
                int w, int h,
                bool respect_alpha) {
  
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
    x0 += diff;
    w  -= diff ;
    x   = 0;
  }
  if (y < 0) {
    // Trim bottom of src
    int diff = -y ;
    y0 += diff;
    h  -= diff ;
    y   = 0;
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
      int y1 = y0 + yoff ;
      for (int xoff = 0; xoff < w; xoff++) {
        uint32_t src_col = src[y1 * src_width + x0 + xoff];
        nr_point(dst, dst_height, dst_width, x + xoff, y + yoff, src_col);
      }
    }
  } else {
    // Blit it via 'memcpy()' trashing any contents
    // Flip y axis. matrices are (0, 0) at top left,
    for (int yoff = 0; yoff < h; yoff++) {
      int y1 = y0 + yoff;
      memcpy(
        dst + (y + yoff) * dst_width + x, 
        src + y1 * src_width + x0, 
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
void blit_core_naive_(uint32_t *dst, int dst_width, int dst_height, int x, int y, 
                      uint32_t *src, int src_width, int src_height, int x0, int y0, 
                      int w, int h) {
  
  for (int yoff = 0; yoff < h; yoff++) {
    int y1 = src_height - y0 - yoff;
    for (int xoff = 0; xoff < w; xoff++) {
      
      uint32_t src_col = src[y1 * src_width + x0 + xoff - 1];
      
      nr_point(dst, dst_height, dst_width, x + xoff, y + yoff, src_col);
    }
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprites into raster [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_blit_(SEXP dst_  , SEXP src_, 
             SEXP x_    , SEXP y_, 
             SEXP x0_   , SEXP y0_, 
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
  int x0 = Rf_asInteger(x0_);
  int y0 = Rf_asInteger(y0_);
  
  
  int w = Rf_asInteger(w_) < 0 ? src_width  : Rf_asInteger(w_);
  int h = Rf_asInteger(h_) < 0 ? src_height : Rf_asInteger(h_);
  
  if (x0 + w - 1 >= src_width || y0 + h - 1 >= src_height) {
    Rf_error("Specified src [x0 = %i, y0 = %i] + [w = %i, h = %i] is outside bounds [%i, %i]",
          x0, y0, w, h, src_width, src_height);
  }
  
  int hjust = (int)round(Rf_asReal(hjust_) * (src_width  - 1));
  int vjust = (int)round(Rf_asReal(vjust_) * (src_height - 1));
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Blit mulitple copies
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint32_t *dst = (uint32_t *)INTEGER(dst_);
  uint32_t *src = (uint32_t *)INTEGER(src_);
  bool respect_alpha = Rf_asLogical(respect_alpha_);
  for (int i = 0; i < Rf_length(x_); i++) {
    blit_core_(
      dst, dst_width, dst_height, x[i] - hjust, y[i] - vjust, 
      src, src_width, src_height,           x0,           y0, 
      w, h, respect_alpha);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Tidy and return
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (freex) free(x);
  if (freey) free(y);
  return dst_;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_blit_bulk_(SEXP dst_, SEXP src_, SEXP config_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Check arguments are of correct type
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(dst_);
  if (!Rf_isNewList(src_)) {
    Rf_error("'src' MUST be a list of native rasters");
  }
  int Nsrc = Rf_length(src_);
  for (int i = 0; i < Nsrc; ++i) {
    assert_nativeraster(VECTOR_ELT(src_, i));
  }
  if (!Rf_inherits(config_, "data.frame")) {
    Rf_error("Config must be a data.frame");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack the config arguments and convert to correct type if necessary
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int N = Rf_length(VECTOR_ELT(config_, 0));
  if (N == 0) {
    Rf_warning("Empty 'config'");
    return dst_;
  }
  
  SEXP idx_           = get_df_col_or_error(config_, "idx");
  SEXP x_             = get_df_col_or_error(config_, "x");
  SEXP y_             = get_df_col_or_error(config_, "y");
  SEXP w_             = get_df_col_or_error(config_, "w");
  SEXP h_             = get_df_col_or_error(config_, "h");
  SEXP x0_            = get_df_col_or_error(config_, "x0");
  SEXP y0_            = get_df_col_or_error(config_, "y0");
  SEXP hjust_         = get_df_col_or_error(config_, "hjust");
  SEXP vjust_         = get_df_col_or_error(config_, "vjust");
  SEXP respect_alpha_ = get_df_col_or_error(config_, "respect_alpha");
  SEXP render_        = get_df_col_or_error(config_, "render");
  
  bool freeidx = false;
  bool freex = false, freey = false, freex0 = false, freey0 = false;
  bool freew = false, freeh = false;
  
  int *idx   = as_int32_vec(idx_, N, &freeidx);
  int *x     = as_int32_vec(x_  , N, &freex);
  int *y     = as_int32_vec(y_  , N, &freey);
  int *w     = as_int32_vec(w_  , N, &freew);
  int *h     = as_int32_vec(h_  , N, &freeh);
  int *x0    = as_int32_vec(x0_ , N, &freex0);
  int *y0    = as_int32_vec(y0_ , N, &freey0);
  
  double *hjust = REAL(hjust_);
  double *vjust = REAL(vjust_);
  int *respect_alpha = LOGICAL(respect_alpha_);
  int *render        = LOGICAL(render_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Do the loop
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint32_t *dst = (uint32_t *)INTEGER(dst_);
  int dst_width  = Rf_ncols(dst_);
  int dst_height = Rf_nrows(dst_);
  
  for (int i = 0; i < N; ++i) {
    if (!render[i]) continue;
    int this_idx = idx[i] - 1;
    if (idx[i] < 0 || idx[i] >= Nsrc) {
      Rf_error("idx = %i at row %i is invalid. Not in range [1, %i]", idx[i], i, N);
      // continue;
    }
    SEXP this_src_ = VECTOR_ELT(src_, this_idx);
    uint32_t *src = (uint32_t *)INTEGER(this_src_);
    int src_width  = Rf_ncols(this_src_);
    int src_height = Rf_nrows(this_src_);

    int this_hjust = (int)round(hjust[i] * (src_width  - 1));
    int this_vjust = (int)round(vjust[i] * (src_height - 1));

    int this_w = w[i] < 0 ? src_width  : w[i];
    int this_h = h[i] < 0 ? src_height : h[i];
    
    blit_core_(
      dst, dst_width, dst_height, x[i] - this_hjust, y[i] - this_vjust,
      src, src_width, src_height,           x0[i],           y0[i],
      this_w, this_h, respect_alpha[i]);
  }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Tidy and return
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (freeidx) free(idx);
  if (freex)   free(x);
  if (freey)   free(y);
  if (freew)   free(w);
  if (freeh)   free(h);
  if (freex0)  free(x0);
  if (freey0)  free(y0);
  return dst_;
}











