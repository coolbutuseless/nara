


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "colour.h"
#include "nr-utils.h"
#include "nr-draw.h"

#define TRANSPARENT 0
#define HOTPINK -4953601
#define BLUE -65536
#define GREY90 -1710619
#define GREY70 -5000269
#define GREY50 -8421505

void draw_hline_c(int *nr, unsigned int height, unsigned int width, int colour, int y, int x0, int x1, int op) {
  for (int x = x0; x <= x1; x++) {
    draw_point_c(nr, height, width, colour, x, y, op);
  }
}


void draw_vline_c(int *nr, unsigned int height, unsigned int width, int colour, int x, int y0, int y1, int op) {
  for (int y = y0; y <= y1; y++) {
    draw_point_c(nr, height, width, colour, x, y, op);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit isocube onto raster [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void isocube_core_(SEXP nr_, int x, int y, int ysize, int fill_top, int fill_left, int fill_right) {

  int *nr = INTEGER(nr_);

  SEXP nr_dim_ = PROTECT(GET_DIM(nr_));
  int  nr_height = INTEGER(nr_dim_)[0];
  int  nr_width  = INTEGER(nr_dim_)[1];
  UNPROTECT(1);

  // 1:2 gradient in the sequence 1, 2, 2, 3, 3, 4, 4, ....
  int prev = 1;
  int now  = 1;

  for (int i=0; i<ysize; i++) {
    draw_vline_c(nr, nr_height, nr_width, fill_left , x + ysize - i - 1, y + now - 1, y + now + ysize - 2, OP_DRAW); // bot left
    draw_vline_c(nr, nr_height, nr_width, fill_right, x + ysize + i    , y + now - 1, y + now + ysize - 2, OP_DRAW); // bot right

    if (i != (ysize-1)) {
      draw_vline_c(nr, nr_height, nr_width, fill_top, x + ysize - i - 1, y + now + ysize - 1, y + 2*ysize - now, OP_DRAW); // top left
      draw_vline_c(nr, nr_height, nr_width, fill_top, x + ysize + i    , y + now + ysize - 1, y + 2*ysize - now, OP_DRAW); // top right
    }

    if (now == prev) {
      now++;
    } else {
      prev = now;
    }

  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit isocube into raster [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP isocube_(SEXP nr_, SEXP x_, SEXP y_, SEXP ysize_, SEXP fill_top_, SEXP fill_left_, SEXP fill_right_) {

  assert_nativeraster(nr_);

  if (length(x_) != length(y_)) {
    error("'x' and 'y' must be the same length.");
  }
  if (!(length(fill_top_) == length(x_) || length(fill_top_) == 1)) {
    error("'fill_top' must be same length as 'x' or length=1");
  }
  if (!(length(ysize_) == length(x_) || length(ysize_) == 1)) {
    error("'ysize' must be same length as 'x' or length=1");
  }
  if (!(length(fill_left_) == length(x_) || length(fill_left_) == 1)) {
    error("'fill_left' must be same length as 'x' or length=1");
  }
  if (!(length(fill_right_) == length(x_) || length(fill_right_) == 1)) {
    error("'fill_right' must be same length as 'x' or length=1");
  }

  int freex = 0, freey = 0, freeysize = 0;
  int *x     = dbl_to_int(x_    , &freex);
  int *y     = dbl_to_int(y_    , &freey);
  int *ysize = dbl_to_int(ysize_, &freeysize);

  int fill_top   = colour_to_integer(fill_top_);
  int fill_left  = colour_to_integer(fill_left_);
  int fill_right = colour_to_integer(fill_right_);


  for (int i = 0; i < length(x_); i++) {
    isocube_core_(
      nr_,
      x[i],
      y[i],
      length(ysize_)      == 1 ? ysize[0]   : ysize[i],
      length(fill_top_)   == 1 ? fill_top   : colour_to_integer(STRING_ELT(fill_top_  , i)),
      length(fill_left_)  == 1 ? fill_left  : colour_to_integer(STRING_ELT(fill_left_ , i)),
      length(fill_right_) == 1 ? fill_right : colour_to_integer(STRING_ELT(fill_right_, i))
    );
  }

  if (freex)     free(x);
  if (freey)     free(y);
  if (freeysize) free(ysize);
  return nr_;
}









