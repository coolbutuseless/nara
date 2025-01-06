
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw line [C interface]
// 
// @param nr native raster (modified in-place)
// @param nr_width,nr_height dimensions
// @param x1,y1,x2,y2 endpoints of line
// @param color color
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_line(uint32_t *nr, int nr_width, int nr_height, int x1, int y1, int x2, int y2, uint32_t color, double linewidth) {

  if (linewidth > 1) {
    bool close = false;
    double mitre_limit = -1; // doesn't matter. no mitre-ing for a simple line.
    int npoints = 2;
    int x[2] = {x1, x2};
    int y[2] = {y1, y2};
    nr_polyline(nr, nr_width, nr_height, x, y, npoints, color, linewidth, mitre_limit, close);
    return;
  }
  
  
  int dx =  abs(x2-x1), sx = x1<x2 ? 1 : -1;
  int dy = -abs(y2-y1), sy = y1<y2 ? 1 : -1;
  int err = dx+dy, e2;                                  /* error value e_xy */

  for (;;) {                                                        /* loop */
    nr_point(nr, nr_width, nr_height, x1, y1, color);

    e2 = 2*err;
    if (e2 >= dy) {                                       /* e_xy+e_x > 0 */
      if (x1 == x2) break;
      err += dy; x1 += sx;
    }
    if (e2 <= dx) {                                       /* e_xy+e_y < 0 */
      if (y1 == y2) break;
      err += dx; y1 += sy;
    }
  }

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw lines. Vectorised [R interface]
// 
// @param nr native raster (modified in-place)
// @param x1,y1,x2,y2 line endpoints
// @param color colors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_line_(SEXP nr_, SEXP x1_, SEXP y1_, SEXP x2_, SEXP y2_, SEXP color_, SEXP linewidth_) {

  assert_nativeraster(nr_);

  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  
  int nr_height = Rf_nrows(nr_);
  int nr_width  = Rf_ncols(nr_);

  // get an int* from a numeric from R
  bool freex1 = false, freey1 = false, freex2 = false, freey2 = false;
  bool freethickness = false;
  int N = calc_max_length(5, x1_, y1_, x2_, y2_, linewidth_);
  int *x1 = as_int32_vec(x1_, N, &freex1);
  int *y1 = as_int32_vec(y1_, N, &freey1);
  int *x2 = as_int32_vec(x2_, N, &freex2);
  int *y2 = as_int32_vec(y2_, N, &freey2);
  double *linewidth = as_double_vec(linewidth_, N, &freethickness);
  
  // Colors
  bool freecol = false;
  uint32_t *color = multi_rcolors_to_ints(color_, N, &freecol);
  
  for (int i = 0; i < N; i++) {
    nr_line(nr, nr_width, nr_height, x1[i], y1[i], x2[i], y2[i], color[i], linewidth[i]);
  }

  // free and return
  if (freex1) free(x1);
  if (freey1) free(y1);
  if (freex2) free(x2);
  if (freey2) free(y2);
  if (freecol) free(color);
  if (freethickness) free(linewidth);
  return nr_;
}


