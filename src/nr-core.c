


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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copy the contents of one nativeraster into another. [C interface]
// Sizes must match
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void copy_into_c(uint32_t *nr_dst, uint32_t *nr_src, int height, int width) {
  memcpy(nr_dst, nr_src, (size_t)height * (size_t)width * sizeof(uint32_t));
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copy the contents of one nativeraster into another. [R interface]
// Sizes must match
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP copy_into_(SEXP nr_dst_, SEXP nr_src_) {

  assert_nativeraster(nr_src_);
  assert_nativeraster(nr_dst_);

  // Check dims of src and dst
  int  src_height = Rf_nrows(nr_src_);
  int  src_width  = Rf_ncols(nr_src_);

  int  dst_height = Rf_nrows(nr_dst_);
  int  dst_width  = Rf_ncols(nr_dst_);

  if (src_height != dst_height || src_width != dst_width) {
    error("src and dst nativeRaster objects must be the same dimensions");
  }

  uint32_t *nr_src = (uint32_t *)INTEGER(nr_src_);
  uint32_t *nr_dst = (uint32_t *)INTEGER(nr_dst_);

  copy_into_c(nr_dst, nr_src, src_height, src_width);

  return nr_dst_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Create/allocate a new nativeraster and copy the contents of the original
// [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP duplicate_(SEXP nr_) {

  assert_nativeraster(nr_);

  // Get dims of src
  int  height = Rf_nrows(nr_);
  int  width  = Rf_ncols(nr_);

  // Create nativeraster and copy contents
  SEXP nr_new_ = PROTECT(allocVector(INTSXP, width * height));
  copy_into_c((uint32_t *)INTEGER(nr_new_), (uint32_t *)INTEGER(nr_), height, width);

  // Assign correct classes
  SET_CLASS(nr_new_, mkString("nativeRaster"));
  SET_DIM(nr_new_, GET_DIM(nr_));

  UNPROTECT(1);
  return nr_new_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Fill nativeraster with value [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void fill_c(uint32_t *nr, int height, int width, uint32_t color) {
  for (int i = 0; i < height * width; i++) {
    nr[i] = color;
  }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Fill raster with value - R interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP fill_(SEXP nr_, SEXP color_) {

  assert_nativeraster(nr_);

  uint32_t *nr = (uint32_t *)INTEGER(nr_);

  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);

  uint32_t color = color_sexp_to_packed_col(color_);

  fill_c(nr, height, width, color);

  return nr_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Flip vertically
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP flipv_(SEXP nr_) {
  
  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);
  
  int *tmp = (int *)malloc((size_t)width * sizeof(int));
  if (tmp == NULL) {
    error("flipv_(): malloc() failure");
  }
  
  for (int row = 0; row < height/2; row++) {
    memcpy(tmp                            , nr +                row * width, (size_t)width * sizeof(uint32_t));
    memcpy(nr + row * width               , nr + (height - row - 1) * width, (size_t)width * sizeof(uint32_t));
    memcpy(nr + (height - row - 1) * width,                             tmp, (size_t)width * sizeof(uint32_t));
  }
  
  free(tmp);
  return nr_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Flip horizontally
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



SEXP replace_(SEXP nr_, SEXP old_cols_, SEXP new_cols_) {
  
  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);
  
  int N = calc_max_length(2, old_cols_, new_cols_);
  
  bool freeold = false, freenew = false;
  uint32_t *old_cols = colors_to_packed_cols(old_cols_, N, &freeold);
  uint32_t *new_cols = colors_to_packed_cols(new_cols_, N, &freenew);
  
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      for (int idx = 0; idx < N; idx++) {
        if (*nr == old_cols[idx]) {
          *nr = new_cols[idx];
        }
      }
      nr++;
    }
  }
  
  if (freeold) free(old_cols);
  if (freenew) free(new_cols);
  return nr_;
}

