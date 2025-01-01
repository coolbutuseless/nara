
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw Rect. Vectorised [R interface]
// 
// This is a simplified interface using builtin fonts. 
// Use `naratext` and/or 'lofifonts' for more options
//
// @param nr native raster (modified in-place)
// @param height,width dimensions
// @param x,y locations
// @param w,h widths and heights
// @param fill fill colour
// @param color outline colour
// @param hjust,vjust the handle on the rect.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_rect_(SEXP nr_, SEXP x_, SEXP y_, SEXP w_, SEXP h_,
                SEXP fill_, SEXP color_, SEXP hjust_, SEXP vjust_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack args
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  
  int  nr_height = Rf_nrows(nr_);
  int  nr_width  = Rf_ncols(nr_);
  
  bool freex = false, freey = false, freew = false, freeh = false;
  int N = calc_max_length(4, x_, y_, w_, h_);
  int *xs = as_int32_vec(x_, N, &freex);
  int *ys = as_int32_vec(y_, N, &freey);
  int *ws = as_int32_vec(w_, N, &freew);
  int *hs = as_int32_vec(h_, N, &freeh);
  
  double hjust = Rf_asReal(hjust_);
  double vjust = Rf_asReal(vjust_);
  
  // Colors
  bool freecol = false, freefill = false;
  uint32_t *color = multi_rcolors_to_ints(color_, N, &freecol);
  uint32_t *fill  = multi_rcolors_to_ints(fill_ , N, &freefill);
  
  for (int i = 0; i < N; i++) {
    
    int x = xs[i] - (int)round(hjust * (ws[i] - 1)); // horizontal justification
    int y = ys[i] - (int)round(vjust * (hs[i] - 1)); // vertical justification
    int w = ws[i];
    int h = hs[i];
    
    // Draw Filled rect
    if (!is_transparent(fill[i])) {
      if (is_transparent(color[i])) {
        for (int row = y; row < y + h; row++) {
          nr_hline(nr, nr_height, nr_width, x, x + w - 1, row, fill[i]);
        }
      } else {
        for (int row = y + 1; row < y + h - 1; row++) {
          nr_hline(nr, nr_height, nr_width, x + 1, x + w - 1 - 1, row, fill[i]);
        }
      }
    }
    
    // Draw outline
    if (!is_transparent(color[i])) {
      nr_line(nr, nr_height, nr_width, x    , y  , x+w-1, y    , color[i]);
      nr_line(nr, nr_height, nr_width, x+w-1, y+1, x+w-1, y+h-1, color[i]);
      nr_line(nr, nr_height, nr_width, x+w-2, y+h-1, x+1, y+h-1, color[i]);
      nr_line(nr, nr_height, nr_width, x    , y+h-1, x  , y+1  , color[i]);
    }
  }
  
  
  if (freex) free(xs);
  if (freey) free(ys);
  if (freew) free(ws);
  if (freeh) free(hs);
  if (freecol) free(color);
  if (freefill) free(fill);
  return nr_;
}


