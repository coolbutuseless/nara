


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "colour.h"
#include "nr-utils.h"
#include "nr-draw.h"



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprite onto raster [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void blit_core_(SEXP nr_, int x, int y, SEXP src_, SEXP x0_, SEXP y0_, SEXP w_, SEXP h_) {

  int *nr     = INTEGER(nr_);
  int *sprite = INTEGER(src_);

  SEXP nr_dim_ = PROTECT(GET_DIM(nr_));
  int  nr_height = INTEGER(nr_dim_)[0];
  int  nr_width  = INTEGER(nr_dim_)[1];
  UNPROTECT(1);

  SEXP src_dim_ = PROTECT(GET_DIM(src_));
  int  src_height = INTEGER(src_dim_)[0];
  int  src_width  = INTEGER(src_dim_)[1];
  UNPROTECT(1);

  for (int row = 0; row < src_height; row++) {
    for (int col = 0; col < src_width; col++) {

      int y1 = src_height - row - 1;
      int this_col = sprite[y1 * src_width + col];

      draw_point_c(nr, nr_height, nr_width, this_col, col + x, row + y);
    }
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprites into raster [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP blit_(SEXP nr_, SEXP x_, SEXP y_, SEXP src_, SEXP x0_, SEXP y0_, SEXP w_, SEXP h_) {

  assert_nativeraster(nr_);
  assert_nativeraster(src_);

  if (length(x_) != length(y_)) {
    error("'x' and 'y' must be the same length.");
  }

  int freex = 0, freey = 0;
  int *x = dbl_to_int(x_, &freex);
  int *y = dbl_to_int(y_, &freey);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check the sprite dimensions
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  
  
  for (int i = 0; i < length(x_); i++) {
    blit_core_(nr_, x[i], y[i], src_, x0_, y0_, w_, h_);
  }

  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}









