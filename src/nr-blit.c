


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "colour.h"
#include "nr-utils.h"
#include "nr-draw.h"



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprite onto raster [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void blit_core_(SEXP nr_, SEXP sprite_, int x, int y) {

  int *nr     = INTEGER(nr_);
  int *sprite = INTEGER(sprite_);

  SEXP nr_dim_ = PROTECT(GET_DIM(nr_));
  int  nr_height = INTEGER(nr_dim_)[0];
  int  nr_width  = INTEGER(nr_dim_)[1];
  UNPROTECT(1);

  SEXP sprite_dim_ = PROTECT(GET_DIM(sprite_));
  int  sprite_height = INTEGER(sprite_dim_)[0];
  int  sprite_width  = INTEGER(sprite_dim_)[1];
  UNPROTECT(1);

  for (int row = 0; row < sprite_height; row++) {
    for (int col = 0; col < sprite_width; col++) {

      int y1 = sprite_height - row - 1;
      int this_col = sprite[y1 * sprite_width + col];

      draw_point_c(nr, nr_height, nr_width, this_col, col + x, row + y, OP_DRAW);
    }
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprites into raster [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP blit_(SEXP nr_, SEXP sprite_, SEXP x_, SEXP y_) {

  assert_nativeraster(nr_);
  assert_nativeraster(sprite_);

  if (length(x_) != length(y_)) {
    error("'x' and 'y' must be the same length.");
  }

  int freex = 0, freey = 0;
  int *x = dbl_to_int(x_, &freex);
  int *y = dbl_to_int(y_, &freey);

  for (int i = 0; i < length(x_); i++) {
    blit_core_(nr_, sprite_, x[i], y[i]);
  }

  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}









