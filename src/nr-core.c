
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// R Interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_new_(SEXP nr_width_, SEXP nr_height_) {
  return nr_new(Rf_asInteger(nr_width_), Rf_asInteger(nr_height_));
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copy the contents of one nativeraster into another. [R interface]
// Sizes must match
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP copy_into_(SEXP nr_dst_, SEXP nr_src_) {

  assert_nativeraster(nr_src_);
  assert_nativeraster(nr_dst_);

  // Check dims of src and dst
  size_t  src_height = (size_t)Rf_nrows(nr_src_);
  size_t  src_width  = (size_t)Rf_ncols(nr_src_);
  size_t  dst_height = (size_t)Rf_nrows(nr_dst_);
  size_t  dst_width  = (size_t)Rf_ncols(nr_dst_);

  if (src_height != dst_height || src_width != dst_width) {
    Rf_error("src and dst nativeRaster objects must be the same dimensions");
  }

  memcpy(INTEGER(nr_dst_), INTEGER(nr_src_), src_height * src_width * sizeof(uint32_t));
  
  return nr_dst_;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Create/allocate a new nativeraster and copy the contents of the original
// [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP duplicate_(SEXP nr_) {
  assert_nativeraster(nr_);
  return Rf_duplicate(nr_);
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Fill raster with value - R interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP fill_(SEXP nr_, SEXP color_) {

  assert_nativeraster(nr_);

  uint32_t *nr = (uint32_t *)INTEGER(nr_);

  int N = Rf_length(nr_);

  uint32_t color = single_rcolor_to_int(color_);

  int i = 0;
  for (; i < N - 3; i += 4) {
    nr[i + 0] = color;
    nr[i + 1] = color;
    nr[i + 2] = color;
    nr[i + 3] = color;
  }
  
  for (; i < N; i++) {
    nr[i] = color;
  }

  return nr_;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Flip vertically
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP flipv_(SEXP nr_) {
  
  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  size_t height = (size_t)Rf_nrows(nr_);
  size_t width  = (size_t)Rf_ncols(nr_);
  
  int *tmp = (int *)malloc(width * sizeof(int));
  if (tmp == NULL) {
    Rf_error("flipv_(): malloc() failure");
  }
  
  for (int row = 0; row < height/2; row++) {
    memcpy(tmp                            , nr +                row * width, width * sizeof(uint32_t));
    memcpy(nr + row * width               , nr + (height - row - 1) * width, width * sizeof(uint32_t));
    memcpy(nr + (height - row - 1) * width,                             tmp, width * sizeof(uint32_t));
  }
  
  free(tmp);
  return nr_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Flip horizontally (in-place)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP fliph_(SEXP nr_) {
  
  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);
  
  for (int row = 0; row < height; row++) {
    uint32_t *lo = nr +  row * width;
    uint32_t *hi = lo + width - 1;
    while (lo < hi) {
      uint32_t tmp = *lo;
      *lo++ = *hi;
      *hi-- = tmp;
    }
  }
  
  return nr_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Replace old colours with new colours
// 
// @param nr modified in-place
// @param old_cols vector of old colours
// @param new_cols vector of new colours
//
// @return modified nr
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP replace_(SEXP nr_, SEXP old_cols_, SEXP new_cols_) {
  
  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);
  
  int N = calc_max_length(2, old_cols_, new_cols_);
  
  bool freeold = false, freenew = false;
  uint32_t *old_cols = multi_rcolors_to_ints(old_cols_, N, &freeold);
  uint32_t *new_cols = multi_rcolors_to_ints(new_cols_, N, &freenew);
  
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      for (int idx = 0; idx < N; idx++) {
        if (*nr == old_cols[idx]) {
          *nr = new_cols[idx];
          break;
        }
      }
      nr++;
    }
  }
  
  if (freeold) free(old_cols);
  if (freenew) free(new_cols);
  return nr_;
}

