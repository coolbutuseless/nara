
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

void nr_rect(uint32_t *nr, 
             int nr_width, int nr_height, 
             int x, int y, 
             int w, int h, 
             uint32_t fill, uint32_t color, 
             double hjust, double vjust, 
             double linewidth) {

  // Adjust handle on rectangle
  x = x - (int)round(hjust * (w - 1)); // horizontal justification
  y = y - (int)round(vjust * (h - 1)); // vertical justification
  
  if (!is_transparent(fill)) {
    for (int row = y; row < y + h; row++) {
      nr_hline(nr, nr_width, nr_height, x, x + w - 1, row, fill);
    }
  }
  
  // Draw outline
  if (!is_transparent(color)) {
    int xs[4] = {x, x + w - 1, x + w - 1, x};
    int ys[4] = {y, y, y + h - 1, y + h - 1};
    int npoints = 4;
    bool close = true;
    double mitre_limit = linewidth;
    
    nr_polyline(nr, nr_width, nr_height, xs, ys, npoints, color, linewidth, mitre_limit, close);
    
    // nr_line(nr, nr_width, nr_height, x    , y  , x+w-1, y    , color, linewidth);
    // nr_line(nr, nr_width, nr_height, x+w-1, y+1, x+w-1, y+h-1, color, linewidth);
    // nr_line(nr, nr_width, nr_height, x+w-2, y+h-1, x+1, y+h-1, color, linewidth);
    // nr_line(nr, nr_width, nr_height, x    , y+h-1, x  , y+1  , color, linewidth);
  }
  
}
  
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw Rect. Vectorised [R interface]
//
// @param nr native raster (modified in-place)
// @param width,height dimensions
// @param x,y locations
// @param w,h widths and heights
// @param fill fill colour
// @param color outline colour
// @param hjust,vjust the handle on the rect.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_rect_(SEXP nr_, SEXP x_, SEXP y_, SEXP w_, SEXP h_,
                SEXP fill_, SEXP color_, SEXP hjust_, SEXP vjust_, SEXP thickness_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack args
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  
  int  nr_height = Rf_nrows(nr_);
  int  nr_width  = Rf_ncols(nr_);
  
  bool freex = false, freey = false, freew = false, freeh = false, freethickness = false;
  int N = calc_max_length(5, x_, y_, w_, h_, thickness_);
  int *xs = as_int32_vec(x_, N, &freex);
  int *ys = as_int32_vec(y_, N, &freey);
  int *ws = as_int32_vec(w_, N, &freew);
  int *hs = as_int32_vec(h_, N, &freeh);
  double *linewidth = as_double_vec(thickness_, N, &freethickness);
  
  double hjust = Rf_asReal(hjust_);
  double vjust = Rf_asReal(vjust_);
  
  // Colors
  bool freecol = false, freefill = false;
  uint32_t *color = multi_rcolors_to_ints(color_, N, &freecol);
  uint32_t *fill  = multi_rcolors_to_ints(fill_ , N, &freefill);
  
  
  
  // Draw each rect
  for (int i = 0; i < N; i++) {
    nr_rect(nr, nr_width, nr_height, xs[i], ys[i], ws[i], hs[i], fill[i], color[i], hjust, vjust, linewidth[i]);
  }
  
  
  if (freex) free(xs);
  if (freey) free(ys);
  if (freew) free(ws);
  if (freeh) free(hs);
  if (freecol) free(color);
  if (freefill) free(fill);
  if (freethickness) free(linewidth);
  return nr_;
}


