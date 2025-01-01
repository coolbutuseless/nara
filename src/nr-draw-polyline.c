
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
#include "nr-utils.h"
#include "nr-draw.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a polyline
//
// @param nr native raster (modified in-place)
// @param height,width dimensions
// @param x,y vertices
// @param N number of vertices
// @param close should the polyline be closed?
// @param color color
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_polyline(uint32_t *nr, int height, int width, int *x, int *y, int N, uint32_t color, int close) {
  
  if (is_transparent(color)) return;
  
  // Draw lines between pairs of points
  for (int i = 0; i < N - 1; i++) {
    nr_line(nr, height, width, x[i], y[i], x[i+1], y[i+1], color);
  }
  
  if (close) {
    // Join last point and first point if requested
    nr_line(nr, height, width, x[N - 1], y[N - 1], x[0], y[0], color);
  }
  
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw polyline [R interace]
// 
// @param nr native raster (modified in-place)
// @param height,width dimensions
// @param x,y locations
// @param color colour
// @param close should the polyline be closed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_polyline_(SEXP nr_, SEXP x_, SEXP y_, SEXP color_, SEXP close_) {

  assert_nativeraster(nr_);

  uint32_t *nr = (uint32_t *)INTEGER(nr_);

  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);

  uint32_t color = single_rcolor_to_int(color_);
  
  if (Rf_length(x_) != Rf_length(y_)) {
    Rf_error("Arguments 'x' and 'y' must be same length.");
  }
  
  // get an int* from a numeric from R
  bool freex = false, freey = false;
  int N = calc_max_length(2, x_, y_);
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);
  
  nr_polyline(nr, height, width, x, y, N, color, Rf_asInteger(close_));

  // free and return
  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}


