
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
// @param x0,y0,x1,y1 endpoints of line
// @param color color
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_line(uint32_t *nr, int nr_width, int nr_height, int x0, int y0, int x1, int y1, uint32_t color) {

  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = dx+dy, e2;                                  /* error value e_xy */

  for (;;) {                                                        /* loop */
    nr_point(nr, nr_width, nr_height, x0, y0, color);

    e2 = 2*err;
    if (e2 >= dy) {                                       /* e_xy+e_x > 0 */
      if (x0 == x1) break;
      err += dy; x0 += sx;
    }
    if (e2 <= dx) {                                       /* e_xy+e_y < 0 */
      if (y0 == y1) break;
      err += dx; y0 += sy;
    }
  }

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw lines. Vectorised [R interface]
// 
// @param nr native raster (modified in-place)
// @param x0,y0,x1,y1 line endpoints
// @param color colors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_line_(SEXP nr_, SEXP x0_, SEXP y0_, SEXP x1_, SEXP y1_, SEXP color_) {

  assert_nativeraster(nr_);

  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  
  int nr_height = Rf_nrows(nr_);
  int nr_width  = Rf_ncols(nr_);

  // get an int* from a numeric from R
  bool freex0 = false, freey0 = false, freex1 = false, freey1 = false;
  int N = calc_max_length(4, x0_, y0_, x1_, y1_);
  int *x0 = as_int32_vec(x0_, N, &freex0);
  int *y0 = as_int32_vec(y0_, N, &freey0);
  int *x1 = as_int32_vec(x1_, N, &freex1);
  int *y1 = as_int32_vec(y1_, N, &freey1);
  
  // Colors
  bool freecol = false;
  uint32_t *color = multi_rcolors_to_ints(color_, N, &freecol);

  for (int i = 0; i < N; i++) {
    nr_line(nr, nr_width, nr_height, x0[i], y0[i], x1[i], y1[i], color[i]);
  }

  // free and return
  if (freex0) free(x0);
  if (freey0) free(y0);
  if (freex1) free(x1);
  if (freey1) free(y1);
  if (freecol) free(color);
  return nr_;
}


