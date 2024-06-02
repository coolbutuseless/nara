


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "colour.h"
#include "nr-utils.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copy the contents of one nativeraster into another. [C interface]
// Sizes must match
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void copy_into_c(int *nr_dst, int *nr_src, int height, int width) {
  memcpy(nr_dst, nr_src, (size_t)height * (size_t)width * sizeof(int));
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

  int *nr_src = INTEGER(nr_src_);
  int *nr_dst = INTEGER(nr_dst_);

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
  copy_into_c(INTEGER(nr_new_), INTEGER(nr_), height, width);

  // Assign correct classes
  SET_CLASS(nr_new_, mkString("nativeRaster"));
  SET_ATTR(nr_new_, mkString("channels"), ScalarInteger(4));
  SET_DIM(nr_new_, GET_DIM(nr_));

  UNPROTECT(1);
  return nr_new_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Fill nativeraster with value [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void fill_c(int *nr, int height, int width, int colour) {
  int *nrp = nr;
  for (int i = 0; i < height * width; i++) {
    *nrp++ = colour;
  }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Fill raster with value - R interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP fill_(SEXP nr_, SEXP colour_) {

  assert_nativeraster(nr_);

  int *nr = INTEGER(nr_);

  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);

  int colour = colour_sexp_to_packed_col(colour_);

  fill_c(nr, height, width, colour);

  return nr_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Flip vertically
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP flipv_(SEXP nr_) {
  
  assert_nativeraster(nr_);
  int *nr = INTEGER(nr_);
  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);
  
  int *tmp = (int *)malloc((size_t)width * sizeof(int));
  if (tmp == NULL) {
    error("flipv_(): malloc() failure");
  }
  
  for (int row = 0; row < height/2; row++) {
    memcpy(tmp                            , nr +                row * width, (size_t)width * sizeof(int));
    memcpy(nr + row * width               , nr + (height - row - 1) * width, (size_t)width * sizeof(int));
    memcpy(nr + (height - row - 1) * width,                             tmp, (size_t)width * sizeof(int));
  }
  
  free(tmp);
  return nr_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Flip horizontally
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP fliph_(SEXP nr_) {
  
  assert_nativeraster(nr_);
  int *nr = INTEGER(nr_);
  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);
  
  for (int row = 0; row < height; row++) {
    int *lo = nr +  row * width;
    int *hi = lo + width - 1;
    while (lo < hi) {
      int tmp = *lo;
      *lo++ = *hi;
      *hi-- = tmp;
    }
  }
  
  return nr_;
}





