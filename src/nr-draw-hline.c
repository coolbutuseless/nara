
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a horizontal sequence of points from x1 to x2 INCLUSIVE
// 
// @param nr native raster (modified in-place)
// @param nr_width,nr_height dimensions
// @param y location
// @param x1,x2 points to draw between
// @param color colour
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_hline(uint32_t *nr, int nr_width, int nr_height, int x1, int x2, int y, uint32_t color, draw_mode_t draw_mode) {
  
  if (is_transparent(color) || y < 0 || y >= nr_height) {
    return;
  }
  
  // Guarantee ordering that x1 <= x2
  if (x1 > x2) {
    int tmp = x1;
    x1 = x2;
    x2 = tmp;
  }
  
  // trim to avoid out-of-bound writes
  if (x1 >= nr_width || x2 < 0) return;
  if (x1 < 0) x1 = 0;
  if (x2 >= nr_width) x2 = nr_width - 1;
  
  if (draw_mode == IGNORE_ALPHA || is_opaque(color)) {
    // draw direct. no need to consider alpha
    for (int x = x1; x <= x2; x++) {
      nr[y * nr_width + x] = color;  
    }
  } else {
    // translucent alpha blend
    // from: https://stackoverflow.com/questions/12011081/alpha-blending-2-rgba-colors-in-c
    uint32_t alpha = ((color >> 24) & 255) + 1;
    uint32_t inv_alpha = 256 - alpha + 1;
    uint8_t fgr   =  color        & 255;
    uint8_t fgg   = (color >>  8) & 255;
    uint8_t fgb   = (color >> 16) & 255;
    
    for (int x = x1; x <= x2; x++) {
      
      // Alpha blending
      uint32_t bg = nr[y * nr_width + x];
      uint8_t bgr =  bg        & 255;
      uint8_t bgg = (bg >>  8) & 255;
      uint8_t bgb = (bg >> 16) & 255;
      
      uint8_t r = (uint8_t)((alpha * fgr + inv_alpha * bgr) >> 8);
      uint8_t g = (uint8_t)((alpha * fgg + inv_alpha * bgg) >> 8);
      uint8_t b = (uint8_t)((alpha * fgb + inv_alpha * bgb) >> 8);
      
      nr[y * nr_width + x] = ((uint32_t)r) | ((uint32_t)g << 8) | ((uint32_t)b << 16) | ((uint32_t)0xff << 24);
    }
  }
  
}

