
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
#include "nr-utils.h"
#include "nr-draw.h"

#include "nr-draw-polyline-thick.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a polyline
//
// @param nr native raster (modified in-place)
// @param nr_width,nr_height dimensions
// @param x,y vertices
// @param npoints number of vertices
// @param close should the polyline be closed?
// @param color color
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_polyline(uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color, double thickness, double mitre_limit, bool close) {
  
  if (is_transparent(color)) return;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Draw a thick line
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (thickness > 1) {
     nr_polyline_thick(nr, nr_width, nr_height, x, y, 
                             npoints, color, thickness, mitre_limit, close);
    return;
  } 
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Draw a 1-point line
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  thickness = 1;
  
  // Draw lines between pairs of points
  for (int i = 0; i < npoints - 1; i++) {
    nr_line(nr, nr_width, nr_height, x[i], y[i], x[i+1], y[i+1], color, thickness);
  }
  
  if (close) {
    // Join last point and first point if requested
    nr_line(nr, nr_width, nr_height, x[npoints - 1], y[npoints - 1], x[0], y[0], color, thickness);
  }
  
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw polyline [R interace]
// 
// @param nr native raster (modified in-place)
// @param nr_width,nr_height dimensions
// @param x,y locations
// @param color colour
// @param close should the polyline be closed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_polyline_(SEXP nr_, SEXP x_, SEXP y_, SEXP color_, SEXP thickness_, SEXP mitre_limit_, SEXP close_) {

  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);

  int nr_height = Rf_nrows(nr_);
  int nr_width  = Rf_ncols(nr_);

  uint32_t color = single_rcolor_to_int(color_);
  
  if (Rf_length(x_) != Rf_length(y_)) {
    Rf_error("Arguments 'x' and 'y' must be same length.");
  }
  
  double thickness = Rf_asReal(thickness_);
  double mitre_limit = Rf_asReal(mitre_limit_);
  
  // get an int* from a numeric from R
  bool freex = false, freey = false;
  int N = calc_max_length(2, x_, y_);
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);
  
  nr_polyline(nr, nr_width, nr_height, x, y, N, color, thickness, mitre_limit, Rf_asInteger(close_));

  // free and return
  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}


