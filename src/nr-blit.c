


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
void blit_core_(int *nr, int x, int y, int nr_width, int nr_height, int *src, int x0, int y0, int w, int h, int src_width, int src_height) {


  for (int yoff = 0; yoff < h; yoff++) {
    int y1 = src_height - y0 - yoff;
    for (int xoff = 0; xoff < w; xoff++) {

      int src_col = src[y1 * src_width + x0 + xoff - 1];

      draw_point_c(nr, nr_height, nr_width, src_col, x + xoff, y + yoff);
    }
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprites into raster [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP blit_(SEXP nr_, SEXP x_, SEXP y_, SEXP src_, SEXP x0_, SEXP y0_, SEXP w_, SEXP h_) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  assert_nativeraster(src_);

  if (length(x_) != length(y_)) {
    error("'x' and 'y' must be the same length.");
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Destination deimensions
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int nr_width, nr_height;
  nr_dim(nr_, &nr_width, &nr_height);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Ensure the coordinates are integers
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int freex = 0, freey = 0;
  int *x = dbl_to_int(x_, &freex);
  int *y = dbl_to_int(y_, &freey);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check the sprite dimensions
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int x0 = asInteger(x0_);
  int y0 = asInteger(y0_);
  
  int src_width, src_height;
  nr_dim(src_, &src_width, &src_height);
  
  int w = isNull(w_) ? src_width : asInteger(w_);
  int h = isNull(h_) ? src_height : asInteger(h_);
  
  if (x0 + w - 1 > src_width || y0 + h - 1 > src_height) {
    error("Specified src [x0,y0] + [w,h] is outside bounds");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Blit mulitple copies
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int *nr  = INTEGER(nr_);
  int *src = INTEGER(src_);
  for (int i = 0; i < length(x_); i++) {
    blit_core_(nr, x[i], y[i], nr_width, nr_height, src, x0, y0, w, h, src_width, src_height);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Tidy and return
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}









