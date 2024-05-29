


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "colour.h"
#include "nr-utils.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copy the contents of one nativeraster into another. [C interface]
// Sizes must match
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void copy_into_c(int *nr_dst, int *nr_src, unsigned int height, unsigned int width) {
  memcpy(nr_dst, nr_src, height * width * sizeof(int));
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copy the contents of one nativeraster into another. [R interface]
// Sizes must match
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP copy_into_(SEXP nr_dst_, SEXP nr_src_) {

  assert_nativeraster(nr_src_);
  assert_nativeraster(nr_dst_);

  // Check dims of src and dst
  SEXP src_dim_ = PROTECT(GET_DIM(nr_src_));
  unsigned int  src_height = (unsigned int)INTEGER(src_dim_)[0];
  unsigned int  src_width  = (unsigned int)INTEGER(src_dim_)[1];
  UNPROTECT(1);

  SEXP dst_dim_ = PROTECT(GET_DIM(nr_dst_));
  unsigned int  dst_height = (unsigned int)INTEGER(dst_dim_)[0];
  unsigned int  dst_width  = (unsigned int)INTEGER(dst_dim_)[1];
  UNPROTECT(1);

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
  SEXP dim_ = PROTECT(GET_DIM(nr_));
  unsigned int  height = (unsigned int)INTEGER(dim_)[0];
  unsigned int  width  = (unsigned int)INTEGER(dim_)[1];
  UNPROTECT(1);

  // Create nativeraster and copy contents
  SEXP nr_new_ = PROTECT(allocVector(INTSXP, width * height));
  copy_into_c(INTEGER(nr_new_), INTEGER(nr_), height, width);

  // Assign correct classes
  SET_CLASS(nr_new_, mkString("nativeRaster"));
  SET_ATTR(nr_new_, mkString("channels"), ScalarInteger(4));
  SET_DIM(nr_new_, dim_);

  UNPROTECT(1);
  return nr_new_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Fill nativeraster with value [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void fill_c(int *nr, unsigned int height, unsigned int width, int colour) {
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

  SEXP dim = PROTECT(GET_DIM(nr_));
  unsigned int height = (unsigned int)INTEGER(dim)[0];
  unsigned int width  = (unsigned int)INTEGER(dim)[1];
  UNPROTECT(1);

  int colour = colour_to_integer(colour_);

  fill_c(nr, height, width, colour);

  return nr_;
}






