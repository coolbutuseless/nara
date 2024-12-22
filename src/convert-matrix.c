
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
// Convert a numeric matrix to a native raster using a palette
//
// @param mat_ numeric matrix
// @param palette_ color palette
// @param min,max the limits of the matrix (used for inteprolating the value to a color)
// @param dst optional pre-allocated destination
//
// @return native raster
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP numeric_matrix_to_nr_(SEXP mat_, SEXP palette_, SEXP min_, SEXP max_, SEXP dst_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!Rf_isMatrix(mat_) || !Rf_isReal(mat_)) {
    Rf_error("'mat' must be an numeric matrix");
  }
  
  int N = Rf_length(palette_); // Maximum palette color
  
  int height = Rf_nrows(mat_);
  int width  = Rf_ncols(mat_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Prep native raster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int nprotect = 0;
  
  if (Rf_isNull(dst_)) {
    dst_ = PROTECT(nr_new(height, width)); nprotect++;
  } else {
    assert_nativeraster(dst_);
    if (height != Rf_nrows(dst_) || width !=  Rf_ncols(dst_)) {
      Rf_error("Supplied 'dst' nativeRaster dimenions (%i, %i) do not match source matrix (%i, %i)", 
            Rf_ncols(dst_), Rf_nrows(dst_), height, width);
    }
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Copy the data - from column major matrix to row major nativeRaster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double *mat = REAL(mat_);
  
  bool freepal = false;
  uint32_t *palette = multi_rcolors_to_ints(palette_, N, &freepal);
  
  double min = Rf_asReal(min_);
  double max = Rf_asReal(max_);
  double range_scale = 1.0 / (max - min);
  
  int index = 0;
  uint32_t *ptr = (uint32_t *)INTEGER(dst_);
  
  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      double val = *(mat + col * height + row);
      if (val > max || val < min) {
        Rf_error("Value in matrix out of range (%.1f, %.1f): %.3f", min, max, val);
      }
      size_t idx = (size_t)floor((val - min) * range_scale * N);
      // Rprintf("idx: (%.1f, %.1f)  %.3f = %zu\n", min, max, val, idx);
      ptr[index++] = palette[idx];
    }
  }
  
  
  if (freepal) free(palette);
  UNPROTECT(nprotect);
  return dst_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert a matrix to a native raster using a palette
//
// @param mat_ numeric matrix
// @param palette_ color palette
// @param min,max the limits of the matrix (used for inteprolating the value to a color)
// @param dst optional pre-allocated destination
//
// @return native raster
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP matrix_to_nr_(SEXP mat_, SEXP palette_, SEXP fill_, SEXP min_, SEXP max_, SEXP dst_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!Rf_isMatrix(mat_)) {
    Rf_error("'mat' must be an matrix");
  }
  
  if (Rf_isReal(mat_)) {
    return numeric_matrix_to_nr_(mat_, palette_, min_, max_, dst_);
  } 
  
  if (!Rf_isInteger(mat_)) {
    Rf_error("'mat' must be an integer matrix");
  }
  
  
  int N = Rf_length(palette_); // Maximum palette color
  
  int height = Rf_nrows(mat_);
  int width  = Rf_ncols(mat_);
  
  uint32_t fill = single_rcolor_to_int(fill_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Prep native raster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int nprotect = 0;
  
  if (Rf_isNull(dst_)) {
    dst_ = PROTECT(nr_new(height, width)); nprotect++;
  } else {
    assert_nativeraster(dst_);
    if (height != Rf_nrows(dst_) || width !=  Rf_ncols(dst_)) {
      Rf_error("Supplied 'dst' nativeRaster dimenions (%i, %i) do not match source matrix (%i, %i)", 
            Rf_ncols(dst_), Rf_nrows(dst_), height, width);
    }
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Copy the data - from column major matrix to row major nativeRaster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint32_t *nr = (uint32_t *)INTEGER(dst_);
  int *mat = INTEGER(mat_);
  
  bool freepal = false;
  uint32_t *palette = multi_rcolors_to_ints(palette_, N, &freepal);
  
  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      int val = *(mat + col * height + row);
      if (val > N) {
        Rf_error("not enough colors in palette. Need at least %i but have only %i", val, N);
      }
      if (val <= 0) {
        *(nr + row * width + col) = fill;  
      } else {
        *(nr + row * width + col) = palette[val - 1];
      }
    }
  }
  
  
  if (freepal) free(palette);
  UNPROTECT(nprotect);
  return dst_;
}


