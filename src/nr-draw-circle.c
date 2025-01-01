
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
// Draw circle [C interface]
//
// @param xm,ym centre of circle
// @param r radius
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_circle(uint32_t *nr, int height, int width, int xm, int ym, int r, uint32_t fill, uint32_t color) {    
  // Skip NAs
  if (xm == NA_INTEGER || ym == NA_INTEGER || r == NA_INTEGER) {
    return;
  }
  
  int *ydone = (int *)calloc((size_t)r * 2, sizeof(int));
  if (ydone == NULL) {
    Rf_error("nr_circle(): error allocating 'ydone'");
  }
  
  int x = -r, y = 0, err = 2-2*r; /* II. Quadrant */
  do {
    
    if (!is_transparent(fill) && !ydone[y]) {
      nr_hline(nr, height, width, xm + x, xm - x, ym + y, fill);
      if (y != 0) {
        nr_hline(nr, height, width, xm + x, xm - x, ym - y, fill);
      }
      ydone[y] = 1;
    }
    
    if (!is_transparent(color)) {
      nr_point(nr, height, width, xm-x, ym+y, color); /*   I. Quadrant */
      nr_point(nr, height, width, xm+x, ym+y, color); /*  II. Quadrant */
      if (y != 0) {
        nr_point(nr, height, width, xm-x, ym-y, color); /* III. Quadrant */
        nr_point(nr, height, width, xm+x, ym-y, color); /*  IV. Quadrant */
      }
    }
    
    r = err;
    if (r <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
    if (r > x || err > y) err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
  } while (x < 0);
  
  free(ydone);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw Circle. Vectorised [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_circle_(SEXP nr_, SEXP x_, SEXP y_, SEXP r_, SEXP fill_, SEXP color_) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack args
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);

  int  nr_height = Rf_nrows(nr_);
  int  nr_width  = Rf_ncols(nr_);

  bool freexms = false, freeyms = false, freers = false;
  
  int N = calc_max_length(3, x_, y_, r_);
  int *xms = as_int32_vec(x_, N, &freexms);
  int *yms = as_int32_vec(y_, N, &freeyms);
  int *rs  = as_int32_vec(r_, N, &freers );
  
  // Colors
  bool freecol = false, freefill = false;
  uint32_t *color = multi_rcolors_to_ints(color_, N, &freecol);
  uint32_t *fill  = multi_rcolors_to_ints(fill_ , N, &freefill);

  for (int idx = 0; idx < N; idx++) {
    // Rprintf("idx: %i\n", idx);
    int xm = xms[idx];
    int ym = yms[idx];
    int  r =  rs[idx];
    
    // Skip NAs
    if (xm == NA_INTEGER || ym == NA_INTEGER || r == NA_INTEGER) {
      continue;
    }

    int *ydone = (int *)calloc((size_t)r * 2, sizeof(int));
    if (ydone == NULL) {
      Rf_error("error allocating 'ydone'");
    }
    
    int x = -r, y = 0, err = 2-2*r; /* II. Quadrant */
    do {
      
      if (!is_transparent(fill[idx]) && !ydone[y]) {
        nr_hline(nr, nr_height, nr_width, xm + x, xm - x, ym + y, fill[idx]);
        if (y != 0) {
          nr_hline(nr, nr_height, nr_width, xm + x, xm - x, ym - y, fill[idx]);
        }
        ydone[y] = 1;
      }
      
      if (!is_transparent(color[idx])) {
        nr_point(nr, nr_height, nr_width, xm-x, ym+y, color[idx]); /*   I. Quadrant */
        nr_point(nr, nr_height, nr_width, xm+x, ym+y, color[idx]); /*  II. Quadrant */
        if (y != 0) {
          nr_point(nr, nr_height, nr_width, xm-x, ym-y, color[idx]); /* III. Quadrant */
          nr_point(nr, nr_height, nr_width, xm+x, ym-y, color[idx]); /*  IV. Quadrant */
        }
      }
      
      r = err;
      if (r <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
      if (r > x || err > y) err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
    } while (x < 0);
    
    free(ydone);
  }


  if (freexms) free(xms);
  if (freeyms) free(yms);
  if (freers)  free(rs);
  if (freecol) free(color);
  if (freefill) free(fill);
  return nr_;
}

