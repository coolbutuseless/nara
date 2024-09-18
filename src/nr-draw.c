


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
//  Draw a single point on the canvas [C interface]
// 
// Coordinate origin is (0, 0) at top-left
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_point(uint32_t *nr, int height, int width, int x, int y, uint32_t color) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Out of bounds
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (y < 0 || y > height - 1 || x < 0 || x > width - 1) return;
  
  // Check for transparent color
  if (is_transparent(color)) return;

  // Alpha channel for blending colors
  uint32_t alpha = (color >> 24) & 255;
  
  if (alpha == 255) {
    nr[y * width + x] = color;
  } else {
    // Alpha blending
    uint32_t val = nr[y * width + x];
    uint8_t r2 =  val        & 255;
    uint8_t g2 = (val >>  8) & 255;
    uint8_t b2 = (val >> 16) & 255;
    
    uint8_t r =  color        & 255;
    uint8_t g = (color >>  8) & 255;
    uint8_t b = (color >> 16) & 255;
    
    r = (uint8_t)((alpha * r + (255 - alpha) * r2) / 255);
    g = (uint8_t)((alpha * g + (255 - alpha) * g2) / 255);
    b = (uint8_t)((alpha * b + (255 - alpha) * b2) / 255);
    
    nr[y * width + x] = ((uint32_t)r) | ((uint32_t)g << 8) | ((uint32_t)b << 16) | ((uint32_t)0xff << 24);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a horizontal sequence of points from x1 to x2 INCLUSIVE
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_hline(uint32_t *nr, int height, int width, int x1, int x2, int y, uint32_t color) {
  
  if (is_transparent(color) || y < 0 || y >= height) {
    return;
  }
  
  // Guarantee ordering that x1 <= x2
  if (x1 > x2) {
    int tmp = x1;
    x1 = x2;
    x2 = tmp;
  }
  
  // trim to avoid out-of-bound writes
  if (x1 >= width || x2 < 0) return;
  if (x1 < 0) x1 = 0;
  if (x2 >= width) x2 = width - 1;
  
  if (is_opaque(color)) {
    // draw direct. no need to consider alpha
    for (int x = x1; x <= x2; x++) {
      nr[y * width + x] = color;  
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
      uint32_t bg = nr[y * width + x];
      uint8_t bgr =  bg        & 255;
      uint8_t bgg = (bg >>  8) & 255;
      uint8_t bgb = (bg >> 16) & 255;
      
      uint8_t r = (uint8_t)((alpha * fgr + inv_alpha * bgr) >> 8);
      uint8_t g = (uint8_t)((alpha * fgg + inv_alpha * bgg) >> 8);
      uint8_t b = (uint8_t)((alpha * fgb + inv_alpha * bgb) >> 8);
      
      nr[y * width + x] = ((uint32_t)r) | ((uint32_t)g << 8) | ((uint32_t)b << 16) | ((uint32_t)0xff << 24);
    }
  }
  
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw points [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_point_(SEXP nr_, SEXP x_, SEXP y_, SEXP color_) {

  assert_nativeraster(nr_);

  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  
  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);

  // get an int* from a numeric from R
  bool freex = false, freey = false;
  int N = calc_max_length(2, x_, y_);
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);
  
  // Colors
  bool freecol = false;
  uint32_t *color = colors_to_packed_cols(color_, N, &freecol);

  for (int i = 0 ; i < N; i++) {
    nr_point(nr, height, width, x[i], y[i], color[i]);
  }

  // free and return
  if (freex) free(x);
  if (freey) free(y);
  if (freecol) free(color);
  return nr_;
}






//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw line [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_line(uint32_t *nr, int height, int width, int x0, int y0, int x1, int y1, uint32_t color) {

  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = dx+dy, e2;                                  /* error value e_xy */

  for (;;) {                                                        /* loop */
    nr_point(nr, height, width, x0, y0, color);

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
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_line_(SEXP nr_, SEXP x0_, SEXP y0_, SEXP x1_, SEXP y1_, SEXP color_) {

  assert_nativeraster(nr_);

  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  
  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);

  // get an int* from a numeric from R
  bool freex0 = false, freey0 = false, freex1 = false, freey1 = false;
  int N = calc_max_length(4, x0_, y0_, x1_, y1_);
  int *x0 = as_int32_vec(x0_, N, &freex0);
  int *y0 = as_int32_vec(y0_, N, &freey0);
  int *x1 = as_int32_vec(x1_, N, &freex1);
  int *y1 = as_int32_vec(y1_, N, &freey1);
  
  // Colors
  bool freecol = false;
  uint32_t *color = colors_to_packed_cols(color_, N, &freecol);

  for (int i = 0; i < N; i++) {
    nr_line(nr, height, width, x0[i], y0[i], x1[i], y1[i], color[i]);
  }

  // free and return
  if (freex0) free(x0);
  if (freey0) free(y0);
  if (freex1) free(x1);
  if (freey1) free(y1);
  if (freecol) free(color);
  return nr_;
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw polyline [R interace]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_polyline_(SEXP nr_, SEXP x_, SEXP y_, SEXP color_, SEXP close_) {

  assert_nativeraster(nr_);

  uint32_t *nr = (uint32_t *)INTEGER(nr_);

  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);

  uint32_t color = color_sexp_to_packed_col(color_);
  
  if (length(x_) != length(y_)) {
    error("Arguments 'x' and 'y' must be same length.");
  }
  
  // get an int* from a numeric from R
  bool freex = false, freey = false;
  int N = calc_max_length(2, x_, y_);
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);
  
  // Draw lines between pairs of points
  for (int i = 0; i < N - 1; i++) {
    nr_line(nr, height, width, x[i], y[i], x[i+1], y[i+1], color);
  }

  if (asInteger(close_)) {
    // Join last point and first point if requested
    nr_line(nr, height, width, x[N - 1], y[N - 1], x[0], y[0], color);
  }


  // free and return
  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw Text [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "fonts.h"
void nr_text_basic(uint32_t *nr, int height, int width, int x, int y, const char *str, uint32_t color, int fontsize) {
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Choose font
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint8_t *font = spleen5x8;
  int char_w = 5;
  int char_h = 8;
  
  if (fontsize >= 16) {
    font = spleen8x16;
    char_w = 8;
    char_h = 16;
  } else if (fontsize >= 12) {
    font = spleen6x12;
    char_w = 6;
    char_h = 12;
  }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop over letters
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int nchars = (int)strlen(str);
  int col = 0;
  
  for (int char_idx = 0; char_idx < nchars; char_idx++) {
    int c = str[char_idx] - 32;
    if (c < 0 || c > 94) {
      error("draw_text() only accepts plain ASCII.  Char out of range: %i = %d", c, str[char_idx]);
    }
    
    uint8_t *letter = &font[c * char_h];
    for (int row = 0; row < char_h; row ++) {
      for (int i = 0; i < char_w; i++) {
        if (letter[row] & (1 << (8 - i))) {
          nr_point(nr, height, width, col + i + x, y - char_h + row, color);
        }
      }
    }
    
    col += char_w;
  }
}



SEXP nr_text_basic_(SEXP nr_, SEXP x_, SEXP y_, SEXP str_, SEXP color_, SEXP fontsize_) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack args
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  int  nr_height = Rf_nrows(nr_);
  int  nr_width  = Rf_ncols(nr_);

  uint32_t color = color_sexp_to_packed_col(color_);
  int x = asInteger(x_);
  int y = asInteger(y_);

  const char *str = CHAR(asChar(str_));

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Choose font
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int fontsize = asInteger(fontsize_);
  // uint8_t *font = spleen5x8;
  // int char_w = 5;
  // int char_h = 8;
  // 
  // if (fontsize >= 16) {
  //   font = spleen8x16;
  //   char_w = 8;
  //   char_h = 16;
  // } else if (fontsize >= 12) {
  //   font = spleen6x12;
  //   char_w = 6;
  //   char_h = 12;
  // }

  nr_text_basic(nr, nr_height, nr_width, x, y, str, color, fontsize);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop over letters
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // int nchars = (int)strlen(str);
  // int col = 0;
  // 
  // for (int char_idx = 0; char_idx < nchars; char_idx++) {
  //   int c = str[char_idx] - 32;
  //   if (c < 0 || c > 94) {
  //     error("draw_text() only accepts plain ASCII.  Char out of range: %i = %d", c, str[char_idx]);
  //   }
  // 
  //   uint8_t *letter = &font[c * char_h];
  //   for (int row = 0; row < char_h; row ++) {
  //     for (int i = 0; i < char_w; i++) {
  //       if (letter[row] & (1 << (8 - i))) {
  //         nr_point(nr, nr_height, nr_width, col + i + x, y - char_h + row, color);
  //       }
  //     }
  //   }
  // 
  //   col += char_w;
  // }

  return nr_;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw Rect. Vectorised [R interface]
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
  
  double hjust = asReal(hjust_);
  double vjust = asReal(vjust_);
  
  // Colors
  bool freecol = false, freefill = false;
  uint32_t *color = colors_to_packed_cols(color_, N, &freecol);
  uint32_t *fill  = colors_to_packed_cols(fill_ , N, &freefill);
  
  for (int i = 0; i < N; i++) {
    
    int x = xs[i] - (int)round(hjust * ws[i]); // horizontal justification
    int y = ys[i] - (int)round(vjust * hs[i]); // vertical justification
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



void nr_circle(uint32_t *nr, int height, int width, int xm, int ym, int r, uint32_t fill, uint32_t color) {    
  // Skip NAs
  if (xm == NA_INTEGER || ym == NA_INTEGER || r == NA_INTEGER) {
    return;
  }
  
  int *ydone = (int *)calloc((size_t)r * 2, sizeof(int));
  if (ydone == NULL) {
    error("nr_circle(): error allocating 'ydone'");
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
  uint32_t *color = colors_to_packed_cols(color_, N, &freecol);
  uint32_t *fill  = colors_to_packed_cols(fill_ , N, &freefill);

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
      error("error allocating 'ydone'");
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


static int scanline_sort_x(const void *a, const void *b) {
  return *(int *)a - *(int *)b;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw polygon [C interface]
// public domain code from
// https://www.alienryderflex.com/polygon_fill/
// not as efficient as something with an active edge table but it
// get me 30fps in "Another World" so I'm moving on.  Patches/PR welcomed!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_polygon(uint32_t *nr, int height, int width, int *x, int *y, int npoints, uint32_t color) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Pairs of values in 'nodeX' will have points drawn between them on 
  // a scanline
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int *nodeX = (int *)malloc((size_t)npoints * sizeof(int));
  if (nodeX == NULL) {
    error("fill_polygon_c(): memory allocation failed for 'nodeX'");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Find vertical extents of all the polygons
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int ymin = INT_MAX;
  int ymax = INT_MIN;
  for (int i = 0; i < npoints; i++) {
    if (y[i] < ymin) {
      ymin = y[i];
    }
    if (y[i] > ymax) {
      ymax = y[i];
    }
  }
  
  if (ymin < 0) ymin = 0;
  if (ymax >= height) ymax = height - 1;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Precalc the gradient
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double *m = (double *)malloc((size_t)npoints * sizeof(double));
  if (m == NULL) {
    error("fill_polygon_c(): memory allocation failed for 'm'");
  }
  
  {
    int j = npoints - 1; // last point
    for (int i=0; i < npoints; i++) {
      m[i] = (double)(x[j] - x[i]) / (double)(y[j] - y[i]); 
      j = i; 
    }
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop through all the scanlines in the image which contain polygons
  // This would be faster if this used an Active Edge List
  // i.e. 
  //   - Sort lines by starting y, 
  //   - ignore lines when scanline < ystart
  //   - when scanline = ystart add to list of active edges
  //   - loop to find possible lines which intersect scanline runs faster
  //   - when scanline > yend, remove line from active edge list
  //
  // i.e. an active edge list would limit the search space for matching lines
  //      to just those lines which actually cross the scanline, so the 
  //      for loop would no longer be going through all 'npoints' to find intersects
  //
  // Further: no need to calculate 'nodeX' at each scanline.
  //   nodeX at scanline + 1 = nodeX at scaneline + 'm'
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  for (int scanline = ymin; scanline <= ymax; scanline++) {
    
    //  Build a list of nodes whose line crosses the scanline
    int nodes = 0; 
    int j = npoints - 1; // last point
    for (int i=0; i < npoints; i++) {
      if (((y[i] < scanline) && (y[j] >= scanline)) ||  
          ((y[j] < scanline) && (y[i] >= scanline))) {
        nodeX[nodes++] = (int) (x[i] + (scanline - y[i]) * m[i]); ///(double)(y[j] - y[i]) * (x[j] - x[i])); 
      }
      j = i; 
    }
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Sort the x-coordinates along the scanline
    // void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*))
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    qsort(nodeX, nodes, sizeof(int), scanline_sort_x);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //  Fill the pixels between node pairs.
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (int i = 0; i < nodes; i += 2) {
      if (nodeX[i] >= width) break;
      if (nodeX[i + 1] >  0 ) {
        if (nodeX[i] < 0) nodeX[i] = 0;
        if (nodeX[i + 1] >= width) nodeX[i + 1] = width - 1;
        nr_hline(nr, height, width, nodeX[i], nodeX[i+1], scanline, color);
      }
    }
  } // end for(scanline)
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Tidy
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  free(m);
  free(nodeX);
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// R Polygon [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_polygon_(SEXP nr_, SEXP x_, SEXP y_, SEXP fill_, SEXP color_) {
  
  assert_nativeraster(nr_);
  
  if (length(x_) != length(y_)) {
    error("Arguments 'x' and 'y' must be same length.");
  }
  
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  
  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);
  
  uint32_t color = color_sexp_to_packed_col(color_);
  uint32_t fill   = color_sexp_to_packed_col(fill_);
  
  // get an int* from a numeric from R
  bool freex = false, freey = false;
  int N = calc_max_length(2, x_, y_);
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);
  
  // Rprintf("Polygon Fill: %i\n", fill);
  nr_polygon(nr, height, width, x, y, length(x_), fill);
  
  // outline
  if (!is_transparent(color)) {
    // Rprintf("Polygon Color: %i\n", color);
    nr_polyline_(nr_, x_, y_, color_, ScalarLogical(1));
  }
  
  // free and return
  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// R Polygon [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_polygons_(SEXP nr_, SEXP x_, SEXP y_, SEXP id_, SEXP fill_, SEXP color_) {
  
  // Can we just do single polygon handling?
  if (isNull(id_)) {
    // Rprintf("Calling single\n");
    return nr_polygon_(nr_, x_, y_, fill_, color_);
  }
  // Rprintf("Processing multiple\n");
  
  // Unpack native raster
  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  int height = Rf_nrows(nr_);
  int width  = Rf_ncols(nr_);
  
  // Sanity check
  int N = length(x_);
  if (length(y_) != N) {
    error("Arguments 'x' and 'y' must be same length.");
  }
  
  if (length(id_) != N) {
    error("Must supply 'id' arguments for all coordinates");
  }
  
  // Ensure 'id' is integer type for easier processing
  bool freeid = false;
  int *id = as_int32_vec(id_, N, &freeid);
  
  // How many polygons are defined by 'id'?
  int npolys = 1;
  int last_id = id[0];
  for (int i = 2; i < N; i++) {
    if (id[i] != last_id) {
      last_id = id[i];
      npolys++;
    }
  }
  
  // Rprintf("Number of polygons defined: %i\n", npolys);
  
  // get an int* from a numeric from R
  bool freex = false, freey = false;
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);
  
  // Colors
  bool freecol = false, freefill = false;
  uint32_t *color = colors_to_packed_cols(color_, npolys, &freecol);
  uint32_t *fill  = colors_to_packed_cols(fill_ , npolys, &freefill);
  
  int poly_id = id[0];
  int poly_start = 0;
  int poly_end = 1;
  for (int i = 0; i < npolys - 1; i++) {
    
    while ( id[poly_end] == poly_id ) {
      poly_end++;
    } 
    
    int len = poly_end - poly_start;
    
    // Rprintf("Fill [%i] = %i\n", i, fill[i]);
    nr_polygon(nr, height, width, x + poly_start, y + poly_start, len, fill[i]);
    
    poly_start = poly_end;
    poly_id    = id[poly_start];
    poly_end++;
    // if (!is_transparent(color)) {
    //   nr_polyline_(nr_, x_, y_, color_, ScalarLogical(1));
    // }
  }
  
  
  // Final polygon
  int len = N - poly_start;
  // Rprintf("Final len: %i\n", len);
  nr_polygon(nr, height, width, x + poly_start, y + poly_start, len, fill[npolys - 1]);
  
  
  
  
  // free and return
  if (freex)    free(x);
  if (freey)    free(y);
  if (freecol)  free(color);
  if (freefill) free(fill);
  return nr_;
}

