


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "colour.h"
#include "colour-from-r.h"
#include "nr-utils.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert matrix to native raster with palette
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP matrix_to_nr_(SEXP mat_, SEXP palette_, SEXP dst_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!isMatrix(mat_) || !isInteger(mat_)) {
    error("'mat' must be an integer matrix");
  }
  
  int N = length(palette_); // Maximum palette colour
  
  int height = Rf_nrows(mat_);
  int width  = Rf_ncols(mat_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Prep native raster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int nprotect = 0;
  
  if (isNull(dst_)) {
    dst_ = PROTECT(allocVector(INTSXP, length(mat_))); nprotect++;
    SET_CLASS(dst_, mkString("nativeRaster"));
    SET_ATTR(dst_, mkString("channels"), ScalarInteger(4));
    SET_DIM(dst_, GET_DIM(mat_));
  } else {
    assert_nativeraster(dst_);
    if (height != Rf_nrows(dst_) || width !=  Rf_ncols(dst_)) {
      error("Supplied 'dst' nativeRaster dimenions (%i, %i) do not match source matrix (%i, %i)", 
            Rf_ncols(dst_), Rf_nrows(dst_), height, width);
    }
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Copy the data - from column major matrix to row major nativeRaster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int *nr = INTEGER(dst_);
  int *mat = INTEGER(mat_);
  int *palette = INTEGER(palette_);
  
  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      int val = *(mat + col * height + row);
      if (val > N) {
        error("not enough colours in palette. Need at least %i but have only %i", val, N);
      }
      if (val <= 0) {
        *(nr + row * width + col) = 16777215;  // #ffffff00
      } else {
        *(nr + row * width + col) = palette[val - 1];
      }
    }
  }
  
  
  UNPROTECT(nprotect);
  return dst_;
}
