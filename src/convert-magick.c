
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
// Convert magick image to nativeRaster
//
// @param im_data_ the data part of a magick image: magick::image_data(im)
// @param dst_ optional pre-allocated destination nativeRaster
//
// @return dst_
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP magick_to_nr_(SEXP im_data_, SEXP dst_) {
  
  if (TYPEOF(im_data_) != RAWSXP) {
    Rf_error("Must be a raw vector");
  }
  
  SEXP dim_ = GET_DIM(im_data_);
  if (Rf_length(dim_) != 3) {
    Rf_error("Must be a 3D array");
  }
  
  if (INTEGER(dim_)[0] != 4) {
    Rf_error("Must be an RGBA image");
  }
  
  int width  = INTEGER(dim_)[1];
  int height = INTEGER(dim_)[2];
  
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
  // Copy the RGBA bytes directly into the nativeRaster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  memcpy(INTEGER(dst_), RAW(im_data_), width * height * sizeof(uint32_t));
  
  UNPROTECT(nprotect);
  return dst_;
}

