
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
// Draw a single point on the canvas [C interface]
// 
// @param nr native raster (modified in-place)
// @param nr_width,nr_height dimensions
// @param x,y location
// @param color colour
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_point(uint32_t *nr, int nr_width, int nr_height, int x, int y, uint32_t color) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Out of bounds
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (y < 0 || y > nr_height - 1 || x < 0 || x > nr_width - 1) return;
  
  // Check for transparent color
  if (is_transparent(color)) return;

  // Alpha channel for blending colors
  uint32_t alpha = (color >> 24) & 255;
  
  if (alpha == 255) {
    nr[y * nr_width + x] = color;
  } else {
    // Alpha blending
    uint32_t val = nr[y * nr_width + x];
    uint8_t r2 =  val        & 255;
    uint8_t g2 = (val >>  8) & 255;
    uint8_t b2 = (val >> 16) & 255;
    
    uint8_t r =  color        & 255;
    uint8_t g = (color >>  8) & 255;
    uint8_t b = (color >> 16) & 255;
    
    r = (uint8_t)((alpha * r + (255 - alpha) * r2) / 255);
    g = (uint8_t)((alpha * g + (255 - alpha) * g2) / 255);
    b = (uint8_t)((alpha * b + (255 - alpha) * b2) / 255);
    
    nr[y * nr_width + x] = ((uint32_t)r) | ((uint32_t)g << 8) | ((uint32_t)b << 16) | ((uint32_t)0xff << 24);
  }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw points [R interface]
// 
// @param nr native raster (modified in-place)
// @param x,y locations
// @param color colors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_point_(SEXP nr_, SEXP x_, SEXP y_, SEXP color_) {

  assert_nativeraster(nr_);

  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  
  int nr_height = Rf_nrows(nr_);
  int nr_width  = Rf_ncols(nr_);

  // get an int* from a numeric from R
  bool freex = false, freey = false;
  int N = calc_max_length(2, x_, y_);
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);
  
  // Colors
  bool freecol = false;
  uint32_t *color = multi_rcolors_to_ints(color_, N, &freecol);

  for (int i = 0 ; i < N; i++) {
    nr_point(nr, nr_width, nr_height, x[i], y[i], color[i]);
  }

  // free and return
  if (freex) free(x);
  if (freey) free(y);
  if (freecol) free(color);
  return nr_;
}

