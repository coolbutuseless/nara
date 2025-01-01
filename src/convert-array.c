
#define R_NO_REMAP

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include <colorfast.h>

#include "color.h"
#include "nr-utils.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert RGB or RGBA array to native raster
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP array_to_nr_(SEXP arr_, SEXP dst_) {
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!Rf_isArray(arr_) || !Rf_isNumeric(arr_)) {
    Rf_error("'arr' must be an numeric array");
  }
  
  int height = Rf_nrows(arr_);
  int width  = Rf_ncols(arr_);
  
  SEXP arr_dim = PROTECT(GET_DIM(arr_));
  if (Rf_length(arr_dim) != 3) {
    Rf_error("Only 3-D arrays are accepted");
  }
  int depth = INTEGER(arr_dim)[2];
  if (depth != 3 && depth != 4) {
    Rf_error("'arr' must be RGB or RGBA. Not depth = %i", depth);
  }
  UNPROTECT(1);
  
  int has_alpha = depth == 4;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Prep native raster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int nprotect = 0;
  
  if (Rf_isNull(dst_)) {
    dst_ = PROTECT(nr_new(width, height)); nprotect++;
  } else {
    assert_nativeraster(dst_);
    if (height != Rf_nrows(dst_) || width !=  Rf_ncols(dst_)) {
      Rf_error("Supplied 'dst' nativeRaster dimensions (w:%i, h:%i) do not match source matrix (w:%i, h:%i)", 
            Rf_ncols(dst_), Rf_nrows(dst_), width, height);
    }
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Copy the data - from column major matrix to row major nativeRaster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint32_t *nr = (uint32_t *)INTEGER(dst_);
  double *r = REAL(arr_) + width * height * 0;
  double *g = REAL(arr_) + width * height * 1;
  double *b = REAL(arr_) + width * height * 2;
  double *a = REAL(arr_) + width * height * 3;
  
  if (has_alpha) {
    for (int col = 0; col < width; col++) {
      for (int row = 0; row < height; row++) {
        *(nr + row * width + col) = 
          ((uint32_t)(*(a + col * height + row) * 255) << 24) +
          ((uint32_t)(*(b + col * height + row) * 255) << 16) + 
          ((uint32_t)(*(g + col * height + row) * 255) <<  8) +
          ((uint32_t)(*(r + col * height + row) * 255)      );
      }
    }
  } else {
    for (int col = 0; col < width; col++) {
      for (int row = 0; row < height; row++) {
        *(nr + row * width + col) = 
          ((uint32_t)0xff << 24) +
          ((uint32_t)(*(b + col * height + row) * 255) << 16) + 
          ((uint32_t)(*(g + col * height + row) * 255) <<  8) +
          ((uint32_t)(*(r + col * height + row) * 255)      );
      }
    }
  }
  
  
  UNPROTECT(nprotect);
  return dst_;
}





//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// nativeRaster to array (depth = 4)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_to_array_(SEXP nr_) {
  assert_nativeraster(nr_);
  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);
  
  SEXP arr_ = PROTECT(Rf_allocVector(REALSXP, height * width * 4));
  SET_CLASS(arr_, Rf_mkString("array"));
  
  SEXP arr_dim = PROTECT(Rf_allocVector(INTSXP, 3));
  INTEGER(arr_dim)[0] = height;
  INTEGER(arr_dim)[1] = width;
  INTEGER(arr_dim)[2] = 4;
  SET_DIM(arr_, arr_dim);
  UNPROTECT(1);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Copy the data - from column major matrix to row major nativeRaster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  double *r = REAL(arr_) + width * height * 0;
  double *g = REAL(arr_) + width * height * 1;
  double *b = REAL(arr_) + width * height * 2;
  double *a = REAL(arr_) + width * height * 3;
  
  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      uint32_t val = *(nr + row * width + col);
      a[col * height + row] = (double)( (val >> 24) & 0xff ) / 255;
      b[col * height + row] = (double)( (val >> 16) & 0xff ) / 255;
      g[col * height + row] = (double)( (val >>  8) & 0xff ) / 255;
      r[col * height + row] = (double)( (val      ) & 0xff ) / 255;
    }
  }
  
  UNPROTECT(1);
  return arr_;
}













