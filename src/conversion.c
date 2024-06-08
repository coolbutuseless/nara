


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "color.h"
#include "color-from-r.h"
#include "nr-utils.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert matrix to native raster with palette
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP matrix_to_nr_(SEXP mat_, SEXP palette_, SEXP fill_, SEXP dst_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!isMatrix(mat_) || !isInteger(mat_)) {
    error("'mat' must be an integer matrix");
  }
  
  int N = length(palette_); // Maximum palette color
  
  int height = Rf_nrows(mat_);
  int width  = Rf_ncols(mat_);
  
  uint32_t fill = color_sexp_to_packed_col(fill_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Prep native raster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int nprotect = 0;
  
  if (isNull(dst_)) {
    dst_ = PROTECT(allocVector(INTSXP, length(mat_))); nprotect++;
    SET_CLASS(dst_, mkString("nativeRaster"));
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
  uint32_t *nr = (uint32_t *)INTEGER(dst_);
  int *mat = INTEGER(mat_);
  
  bool freepal = false;
  uint32_t *palette = colors_to_packed_cols(palette_, N, &freepal);
  
  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      int val = *(mat + col * height + row);
      if (val > N) {
        error("not enough colors in palette. Need at least %i but have only %i", val, N);
      }
      if (val <= 0) {
        *(nr + row * width + col) = fill;  // transparent white 0xAABBGGRR
      } else {
        *(nr + row * width + col) = palette[val - 1];
      }
    }
  }
  
  
  if (freepal) free(palette);
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
  uint32_t *nr = (uint32_t *)INTEGER(dst_);

  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      const char *val = CHAR(STRING_ELT(ras_, col * height + row));
      *(nr + col * height + row) = color_char_to_packed_col(val);
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
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  
  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      SET_STRING_ELT(ras_, col * height + row, packed_col_to_CHARSXP_color(nr + col * height + row));
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
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP magick_to_nr_(SEXP im_data_, SEXP dst_) {
  
  if (TYPEOF(im_data_) != RAWSXP) {
    error("Must be a raw vector");
  }
  
  SEXP dim_ = GET_DIM(im_data_);
  if (length(dim_) != 3) {
    error("Must be a 3D array");
  }
  
  if (INTEGER(dim_)[0] != 4) {
    error("Must be an RGBA image");
  }
  
  int width  = INTEGER(dim_)[1];
  int height = INTEGER(dim_)[2];
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Prep native raster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int nprotect = 0;
  
  if (isNull(dst_)) {
    dst_ = PROTECT(allocVector(INTSXP, width * height)); nprotect++;
    SET_CLASS(dst_, mkString("nativeRaster"));
    SEXP new_dim_ = PROTECT(allocVector(INTSXP, 2));
    INTEGER(new_dim_)[0] = height;
    INTEGER(new_dim_)[1] = width;
    SET_DIM(dst_, new_dim_);
    UNPROTECT(1);
  } else {
    assert_nativeraster(dst_);
    if (height != Rf_nrows(dst_) || width !=  Rf_ncols(dst_)) {
      error("Supplied 'dst' nativeRaster dimenions (%i, %i) do not match source matrix (%i, %i)", 
            Rf_ncols(dst_), Rf_nrows(dst_), height, width);
    }
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Copy the RGBA bytes directly into the nativeRaster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  memcpy(INTEGER(dst_), RAW(im_data_), width * height * 4);
  
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













