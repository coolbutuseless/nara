


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "colour.h"
#include "nr-utils.h"
#include "nr-draw.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Draw a single point on the canvas [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void draw_point_c(int *nr, uint32_t height, uint32_t width, int colour, int x, int y) {

  // Check for transparent colour
  if (is_transparent(colour)) return;

  // Flip y axis. matrices are (1, 1) at top left, we want nara plotted to be
  // (1, 1) at bottom-left
  y = height - y;
  x = x - 1;

  // Alpha channel for blending colours
  int alpha = (colour >> 24) & 255;

  if (y >= 0 && y < height && x >= 0 && x < width) {
      if (alpha == 255) {
        nr[y * width + x] = colour;
      } else {
        // Alpha blending
        int val = nr[y * width + x];
        uint8_t r2 =  val        & 255;
        uint8_t g2 = (val >>  8) & 255;
        uint8_t b2 = (val >> 16) & 255;
        // uint8_t a2 = (val >> 24) & 255;

        uint8_t r =  colour        & 255;
        uint8_t g = (colour >>  8) & 255;
        uint8_t b = (colour >> 16) & 255;
        // uint8_t a = (colour >> 24) & 255;

        r = (alpha * r + (255 - alpha) * r2) / 255;
        g = (alpha * g + (255 - alpha) * g2) / 255;
        b = (alpha * b + (255 - alpha) * b2) / 255;
        // a = (alpha * a + (255 - alpha) * a2) / 255;

        nr[y * width + x] = (r) | (g << 8) | (b << 16) | (255 << 24);
      }
  }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw multiple points on the canvas [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void draw_points_c(int *nr, uint32_t height, uint32_t width, int colour, int *x, int *y, int npoints) {
  for (int i = 0 ; i < npoints; i++) {
    draw_point_c(nr, height, width, colour, x[i], y[i]);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw points [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP draw_points_(SEXP nr_, SEXP x_, SEXP y_, SEXP colour_) {

  assert_nativeraster(nr_);

  if (length(x_) != length(y_)) {
    error("x and y must be same length");
  }

  int *nr = INTEGER(nr_);
  
  uint32_t height = (uint32_t)Rf_nrows(nr_);
  uint32_t width  = (uint32_t)Rf_ncols(nr_);

  // get an int* from a numeric from R
  bool freex = false, freey = false;
  int N = calc_max_length(2, x_, y_);
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);
  
  // Colours
  bool freecol = false;
  int *colour = colours_to_packed_cols(colour_, N, &freecol);

  for (int i = 0 ; i < N; i++) {
    draw_point_c(nr, height, width, colour[i], x[i], y[i]);
  }

  // free and return
  if (freex) free(x);
  if (freey) free(y);
  if (freecol) free(colour);
  return nr_;
}






//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw line [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void draw_line_c(int *nr, uint32_t height, uint32_t width, int colour, int x0, int y0, int x1, int y1) {

  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = dx+dy, e2;                                  /* error value e_xy */

  for (;;) {                                                        /* loop */
    draw_point_c(nr, height, width, colour, x0, y0);

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
SEXP draw_line_(SEXP nr_, SEXP x0_, SEXP y0_, SEXP x1_, SEXP y1_, SEXP colour_) {

  assert_nativeraster(nr_);

  int *nr = INTEGER(nr_);
  
  uint32_t height = (uint32_t)Rf_nrows(nr_);
  uint32_t width  = (uint32_t)Rf_ncols(nr_);

  // get an int* from a numeric from R
  bool freex0 = false, freey0 = false, freex1 = false, freey1 = false;
  int N = calc_max_length(4, x0_, y0_, x1_, y1_);
  int *x0 = as_int32_vec(x0_, N, &freex0);
  int *y0 = as_int32_vec(y0_, N, &freey0);
  int *x1 = as_int32_vec(x1_, N, &freex1);
  int *y1 = as_int32_vec(y1_, N, &freey1);
  
  // Colours
  bool freecol = false;
  int *colour = colours_to_packed_cols(colour_, N, &freecol);

  for (int i = 0; i < N; i++) {
    draw_line_c(nr, height, width, colour[i], x0[i], y0[i], x1[i], y1[i]);
  }

  // free and return
  if (freex0) free(x0);
  if (freey0) free(y0);
  if (freex1) free(x1);
  if (freey1) free(y1);
  if (freecol) free(colour);
  return nr_;
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw polyline [R interace]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP draw_polyline_(SEXP nr_, SEXP x_, SEXP y_, SEXP colour_, SEXP close_) {

  assert_nativeraster(nr_);

  int *nr = INTEGER(nr_);

  uint32_t height = (uint32_t)Rf_nrows(nr_);
  uint32_t width  = (uint32_t)Rf_ncols(nr_);

  int colour = colour_sexp_to_packed_col(colour_);
  
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
    draw_line_c(nr, height, width, colour, x[i], y[i], x[i+1], y[i+1]);
  }

  if (asInteger(close_)) {
    // Join last point and first point if requested
    draw_line_c(nr, height, width, colour, x[N - 1], y[N - 1], x[0], y[0]);
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
SEXP draw_text_(SEXP nr_, SEXP x_, SEXP y_, SEXP str_, SEXP colour_, SEXP fontsize_) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack args
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  int *nr = INTEGER(nr_);
  int  nr_height = Rf_nrows(nr_);
  int  nr_width  = Rf_ncols(nr_);

  int colour = colour_sexp_to_packed_col(colour_);
  int x = asInteger(x_);
  int y = asInteger(y_);

  const char *str = CHAR(asChar(str_));

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Choose font
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int fontsize = asInteger(fontsize_);
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
  int nchars = strlen(str);
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
          draw_point_c(nr, nr_height, nr_width, colour, col + i + x, y + (char_h - 1) - row);
        }
      }
    }

    col += char_w;
  }

  return nr_;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw Rect. Vectorised [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP draw_rect_(SEXP nr_, SEXP x_, SEXP y_, SEXP w_, SEXP h_,
                SEXP fill_, SEXP colour_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack args
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  int *nr = INTEGER(nr_);
  
  int  nr_height = Rf_nrows(nr_);
  int  nr_width  = Rf_ncols(nr_);
  
  bool freex = false, freey = false, freew = false, freeh = false;
  int N = calc_max_length(4, x_, y_, w_, h_);
  int *xs = as_int32_vec(x_, N, &freex);
  int *ys = as_int32_vec(y_, N, &freey);
  int *ws = as_int32_vec(w_, N, &freew);
  int *hs = as_int32_vec(h_, N, &freeh);
  
  // Colours
  bool freecol = false, freefill = false;
  int *colour = colours_to_packed_cols(colour_, N, &freecol);
  int *fill   = colours_to_packed_cols(fill_  , N, &freefill);
  
  for (int i = 0; i < N; i++) {
    
    int x = xs[i];
    int y = ys[i];
    int w = ws[i] - 1;
    int h = hs[i] - 1;
    
    // Draw Filled rect
    if (!is_transparent(fill[i])) {
      for (int row = y; row <= y + h; row++) {
        for (int col = x; col <= x + w; col++) {
          draw_point_c(nr, nr_height, nr_width,  fill[i], col, row);
        }
      }
    }
    
    // Draw outline
    if (!is_transparent(colour[i])) {
      draw_line_c(nr, nr_height, nr_width, colour[i], x    , y  , x+w, y  );
      draw_line_c(nr, nr_height, nr_width, colour[i], x+w  , y+1, x+w, y+h);
      draw_line_c(nr, nr_height, nr_width, colour[i], x+w-1, y+h, x+1, y+h);
      draw_line_c(nr, nr_height, nr_width, colour[i], x    , y+h, x  , y+1);
    }
  }
  
  
  if (freex) free(xs);
  if (freey) free(ys);
  if (freew) free(ws);
  if (freeh) free(hs);
  if (freecol) free(colour);
  if (freefill) free(fill);
  return nr_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw Circle. Vectorised [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP draw_circle_(SEXP nr_, SEXP x_, SEXP y_, SEXP r_, SEXP fill_, SEXP colour_) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack args
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  int *nr = INTEGER(nr_);

  int  nr_height = Rf_nrows(nr_);
  int  nr_width  = Rf_ncols(nr_);

  bool freexms = false, freeyms = false, freers = false;
  
  int N = calc_max_length(3, x_, y_, r_);
  int *xms = as_int32_vec(x_, N, &freexms);
  int *yms = as_int32_vec(y_, N, &freeyms);
  int *rs  = as_int32_vec(r_, N, &freers );
  
  // Colours
  bool freecol = false, freefill = false;
  int *colour = colours_to_packed_cols(colour_, N, &freecol);
  int *fill   = colours_to_packed_cols(fill_  , N, &freefill);

  for (int idx = 0; idx < length(x_); idx++) {
    // Rprintf("idx: %i\n", idx);
    int xm = xms[idx];
    int ym = yms[idx];
    int  r =  rs[idx];

    int *ydone = (int *)calloc(r * 2, sizeof(int));
    if (ydone == NULL) {
      error("error allocating 'ydone'");
    }
    
    int x = -r, y = 0, err = 2-2*r; /* II. Quadrant */
    do {
      
      if (!is_transparent(fill[idx]) && !ydone[y]) {
        for (int i = xm + x; i <= xm - x; i++) {
          draw_point_c(nr, nr_height, nr_width, fill[idx], i, ym + y);
          if (y != 0) {
            draw_point_c(nr, nr_height, nr_width, fill[idx], i, ym - y);
          }
          ydone[y] = 1;
        }
      }
      
      if (!is_transparent(colour[idx])) {
        draw_point_c(nr, nr_height, nr_width, colour[idx], xm-x, ym+y); /*   I. Quadrant */
        draw_point_c(nr, nr_height, nr_width, colour[idx], xm+x, ym+y); /*  II. Quadrant */
        if (y != 0) {
          draw_point_c(nr, nr_height, nr_width, colour[idx], xm-x, ym-y); /* III. Quadrant */
          draw_point_c(nr, nr_height, nr_width, colour[idx], xm+x, ym-y); /*  IV. Quadrant */
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
  if (freecol) free(colour);
  if (freefill) free(fill);
  return nr_;
}









//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw polygon [C interface]
// public domain code from
// https://www.alienryderflex.com/polygon_fill/
// not as efficient as something with an active edge table but it
// get me 30fps in "Another World" so I'm moving on.  Patches/PR welcomed!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void fill_polygon_c(int *nr, int height, int width, int colour, int *x, int *y, int npoints) {
  
  int *nodeX = (int *)malloc(npoints * sizeof(int));
  if (nodeX == NULL) {
    error("nodeX alloc failure");
  }
  
  for (int pixelY = 1; pixelY <= height; pixelY++) {
    
    //  Build a list of nodes.
    int nodes = 0;
    int j = npoints - 1;
    for (int i=0; i<npoints; i++) {
      if ( (y[i]< pixelY && y[j]>= pixelY) ||  (y[j] < pixelY && y[i] >= pixelY) ) {
        nodeX[nodes++]=(int) (x[i] + (pixelY - y[i]) / (double)(y[j] - y[i]) * (x[j] - x[i]));
      } else if (y[i] == pixelY && y[j] == pixelY) {
        // If polygons aren't rendering correctly, this is where the issue is
        // going to be.  There's an off-by-one error in how some horizontal
        // lines are drawn, and I tried to fix it be adding this bit of code
        // Mike.  2022-07-16
        if (nodes == 0 || (nodeX[nodes-1] != x[i] && nodeX[nodes-1] != x[j])) {
          // Rprintf("%i -  %i\n", x[i], x[j]);
          nodeX[nodes++] = x[i];
          nodeX[nodes++] = x[j];
        }
      }
      j=i;
    }
    
    // Rprintf("Nodes: %i\n", nodes);
    
    //  Sort the nodes, via a simple “Bubble” sort.
    int i = 0;
    while (i < nodes - 1) {
      if (nodeX[i] > nodeX[i+1]) {
        int swap=nodeX[i];
        nodeX[i]=nodeX[i+1];
        nodeX[i+1]=swap;
        if (i) i--;
      } else {
        i++;
      }
    }
    
    // Rprintf("Y: %i,  node: %i - %i \n", pixelY, nodeX[0], nodeX[1]);
    
    //  Fill the pixels between node pairs.
    for (i=0; i<nodes; i+=2) {
      if   (nodeX[i  ] > width) break;
      if   (nodeX[i+1] >= 1 ) {
        if (nodeX[i  ] <  1 ) nodeX[i  ] = 1 ;
        if (nodeX[i+1]> width+1) nodeX[i+1]=width+1;
        // Rprintf("Y: %i,  node: %i - %i \n", pixelY, nodeX[i], nodeX[i+1]);
        for (int pixelX = nodeX[i]; pixelX <= nodeX[i+1]; pixelX++) {
          draw_point_c(nr, height, width,  colour, pixelX, pixelY);
        }
      }
    }
  }
  free(nodeX);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// R Polygon [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP draw_polygon_(SEXP nr_, SEXP x_, SEXP y_, SEXP fill_, SEXP colour_) {
  
  assert_nativeraster(nr_);
  
  if (length(x_) != length(y_)) {
    error("Arguments 'x' and 'y' must be same length.");
  }
  
  int *nr = INTEGER(nr_);
  
  uint32_t height = (uint32_t)Rf_nrows(nr_);
  uint32_t width  = (uint32_t)Rf_ncols(nr_);

  int colour = colour_sexp_to_packed_col(colour_);
  int fill   = colour_sexp_to_packed_col(fill_);
  
  // get an int* from a numeric from R
  bool freex = false, freey = false;
  int N = calc_max_length(2, x_, y_);
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);
  
  // Rprintf("Polygon Fill: %i\n", fill);
  fill_polygon_c(nr, height, width, fill, x, y, length(x_));
  
  // outline
  if (!is_transparent(colour)) {
    // Rprintf("Polygon Colour: %i\n", colour);
    draw_polyline_(nr_, x_, y_, colour_, ScalarLogical(1));
  }
  
  // free and return
  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}

