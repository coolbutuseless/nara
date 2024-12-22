
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
// Convert raster to native raster
// 
// @param ras_ Raster object. i.e. character matrix
// @param dst_ Optional pre-allocated destination nativeRaster
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP raster_to_nr_(SEXP ras_, SEXP dst_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!Rf_isMatrix(ras_) || TYPEOF(ras_) != STRSXP) {
    Rf_error("'raster' must be an character matrix");
  }
  
  int height = Rf_nrows(ras_);
  int width  = Rf_ncols(ras_);
  
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

  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      const char *val = CHAR(STRING_ELT(ras_, col * height + row));
      *(nr + col * height + row) = col_to_int(val);
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
  
  SEXP ras_ = PROTECT(Rf_allocMatrix(STRSXP, height, width));
  SEXP class_ = PROTECT(Rf_mkString("raster"));
  SET_CLASS(ras_, class_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Copy the data - from column major matrix to row major nativeRaster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  
  char buf[10];
  for (int col = 0; col < width; col++) {
    for (int row = 0; row < height; row++) {
      int_to_col(nr[col * height + row], buf);
      SET_STRING_ELT(ras_, col * height + row, Rf_mkChar(buf));
    }
  }
  
  UNPROTECT(2);
  return ras_;
}

