


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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert raster to native raster
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP raster_to_nr_(SEXP ras_, SEXP dst_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!isMatrix(ras_) || TYPEOF(ras_) != STRSXP) {
    error("'raster' must be an character matrix");
  }
  
  int height = Rf_nrows(ras_);
  int width  = Rf_ncols(ras_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Prep native raster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int nprotect = 0;
  
  if (isNull(dst_)) {
    dst_ = PROTECT(allocVector(INTSXP, length(ras_))); nprotect++;
    SET_CLASS(dst_, mkString("nativeRaster"));
    SET_ATTR(dst_, mkString("channels"), ScalarInteger(4));
    SET_DIM(dst_, GET_DIM(ras_));
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

  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      const char *val = CHAR(STRING_ELT(ras_, col * height + row));
      // *(nr + row * width + col) = colour_to_integer(val);
      *(nr + col * height + row) = single_str_col_to_int(val);
    }
  }
  
  
  UNPROTECT(nprotect);
  return dst_;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// nativeRaster to raster
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_to_raster_(SEXP nr_) {
  assert_nativeraster(nr_);
  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);
  
  SEXP ras_ = PROTECT(allocVector(STRSXP, height * width));
  SET_CLASS(ras_, mkString("raster"));
  SET_DIM(ras_, GET_DIM(nr_));
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Copy the data - from column major matrix to row major nativeRaster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int *nr = INTEGER(nr_);
  
  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      SET_STRING_ELT(ras_, col * height + row, single_int_to_col_CHARSXP(nr + col * height + row));
    }
  }
  
  UNPROTECT(1);
  return ras_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert RGB or RGBA array to native raster
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP array_to_nr_(SEXP arr_, SEXP dst_) {
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!isArray(arr_) || !isNumeric(arr_)) {
    error("'arr' must be an numeric array");
  }
  
  int height = Rf_nrows(arr_);
  int width  = Rf_ncols(arr_);
  
  SEXP arr_dim = PROTECT(GET_DIM(arr_));
  if (length(arr_dim) != 3) {
    error("Only 3-D arrays are accepted");
  }
  int depth = INTEGER(arr_dim)[2];
  if (depth != 3 && depth != 4) {
    error("'arr' must be RGB or RGBA. Not depth = %i", depth);
  }
  UNPROTECT(1);
  
  int has_alpha = depth == 4;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Prep native raster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int nprotect = 0;
  
  if (isNull(dst_)) {
    dst_ = PROTECT(allocVector(INTSXP, height * width)); nprotect++;
    SET_CLASS(dst_, mkString("nativeRaster"));
    SET_ATTR(dst_, mkString("channels"), ScalarInteger(4));
    SEXP nr_dim = PROTECT(allocVector(INTSXP, 2));
    INTEGER(nr_dim)[0] = height;
    INTEGER(nr_dim)[1] = width;
    SET_DIM(dst_, nr_dim);
    UNPROTECT(1);
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
  double *r = REAL(arr_) + width * height * 0;
  double *g = REAL(arr_) + width * height * 1;
  double *b = REAL(arr_) + width * height * 2;
  double *a = REAL(arr_) + width * height * 3;
  
  if (has_alpha) {
    for (int col = 0; col < width; col++) {
      for (int row = 0; row < height; row++) {
        *(nr + row * width + col) = 
          ((uint8_t)(*(a + col * height + row) * 255) << 24) +
          ((uint8_t)(*(b + col * height + row) * 255) << 16) + 
          ((uint8_t)(*(g + col * height + row) * 255) <<  8) +
          ((uint8_t)(*(r + col * height + row) * 255)      );
      }
    }
  } else {
    for (int col = 0; col < width; col++) {
      for (int row = 0; row < height; row++) {
        *(nr + row * width + col) = 
          (0xff << 24) +
          ((uint8_t)(*(b + col * height + row) * 255) << 16) + 
          ((uint8_t)(*(g + col * height + row) * 255) <<  8) +
          ((uint8_t)(*(r + col * height + row) * 255)      );
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
  
  SEXP arr_ = PROTECT(allocVector(REALSXP, height * width * 4));
  SET_CLASS(arr_, mkString("array"));
  
  SEXP arr_dim = PROTECT(allocVector(INTSXP, 3));
  INTEGER(arr_dim)[0] = height;
  INTEGER(arr_dim)[1] = width;
  INTEGER(arr_dim)[2] = 4;
  SET_DIM(arr_, arr_dim);
  UNPROTECT(1);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Copy the data - from column major matrix to row major nativeRaster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int *nr = INTEGER(nr_);
  double *r = REAL(arr_) + width * height * 0;
  double *g = REAL(arr_) + width * height * 1;
  double *b = REAL(arr_) + width * height * 2;
  double *a = REAL(arr_) + width * height * 3;
  
  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      int val = *(nr + row * width + col);
      a[col * height + row] = (double)( (val >> 24) & 0xff ) / 255;
      b[col * height + row] = (double)( (val >> 16) & 0xff ) / 255;
      g[col * height + row] = (double)( (val >>  8) & 0xff ) / 255;
      r[col * height + row] = (double)( (val      ) & 0xff ) / 255;
    }
  }
  
  UNPROTECT(1);
  return arr_;
}













