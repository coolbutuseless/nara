
#define R_NO_REMAP

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "color.h"
#include "nr-core.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// desaturate: 
// TODO: should really do this in HSV space
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_desaturate_(SEXP nr_, SEXP factor_) {
  
  assert_nativeraster(nr_);
  int width  = Rf_ncols(nr_);
  int height = Rf_nrows(nr_);
  uint8_t *p = (uint8_t *)INTEGER(nr_);
  
  double factor = Rf_asReal(factor_);
  
  
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // uint8_t i = (p[0] + p[1] + p[2]) / 3.0;
      double i = (0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2]) ;
      p[0] = p[0] + (uint8_t)((i - p[0]) * factor);
      p[1] = p[1] + (uint8_t)((i - p[1]) * factor);
      p[2] = p[2] + (uint8_t)((i - p[2]) * factor);
      p += 4;
    }
  }
  
  return nr_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// dither
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_dither_(SEXP nr_, SEXP value_, SEXP algo_) {
  
  assert_nativeraster(nr_);
  int im_width  = Rf_ncols(nr_);
  int im_height = Rf_nrows(nr_);
  uint8_t *p = (uint8_t *)INTEGER(nr_);
  
  int err_width  = im_width  + 2;
  int err_height = im_height + 2;
  double *err = calloc((size_t)err_width * (size_t)err_height, sizeof(double));
  if (err == NULL) Rf_error("nr_dither_(): 'err' allocation failed");
  
  double value = Rf_asReal(value_) * 255;
  
  int type = strcmp(CHAR(STRING_ELT(algo_, 0)), "fs") == 0 ? 0 : 1;
  
  for (int y = 0; y < im_height; y++) {
    for (int x = 0; x < im_width; x++) {
      // double i = (p[0] + p[1] + p[2]) / 3.0;
      double i = (0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2]) ;
      i += err[y * err_width + x];
      double delta;
      if (i > value) {
        delta = i - value;
        p[0] = 255;
        p[1] = 255;
        p[2] = 255;
      } else {
        delta = i;
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
      }
      
      if (type == 0) {
        err[(y + 0) * err_width + (x + 1)] += 7 * delta / 16;
        err[(y + 1) * err_width + (x - 1)] += 3 * delta / 16;
        err[(y + 1) * err_width + (x + 0)] += 5 * delta / 16;
        err[(y + 1) * err_width + (x + 1)] += 1 * delta / 16;
      } else if (type == 1) {
        err[(y + 0) * err_width + (x + 1)] += 1 * delta / 8;
        err[(y + 0) * err_width + (x + 2)] += 1 * delta / 8;
        err[(y + 1) * err_width + (x - 1)] += 1 * delta / 8;
        err[(y + 1) * err_width + (x + 0)] += 1 * delta / 8;
        err[(y + 1) * err_width + (x + 1)] += 1 * delta / 8;
        err[(y + 2) * err_width + (x + 0)] += 1 * delta / 8;
      } else {
        Rf_error("Unknown type: %i", type);
      }
      
      p += 4;
    }
  }
  
  free(err);
  return nr_;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// dither
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_threshold_(SEXP nr_, SEXP value_) {
  
  assert_nativeraster(nr_);
  int width  = Rf_ncols(nr_);
  int height = Rf_nrows(nr_);
  uint8_t *p = (uint8_t *)INTEGER(nr_);
  
  double value = Rf_asReal(value_) * 255;
  
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // double i = (p[0] + p[1] + p[2]) / 3.0;
      double i = (0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2]) ;
      if (i > value) {
        p[0] = 255;
        p[1] = 255;
        p[2] = 255;
      } else {
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
      }
      p += 4;
    }
  }
  
  return nr_;
}

