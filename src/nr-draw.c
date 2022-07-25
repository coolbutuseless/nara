


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "colour.h"
#include "nr-utils.h"
#include "nr-draw.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Draw a single point on the canvas [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void draw_point_c(int *nr, unsigned int height, unsigned int width, int colour, int x, int y, int op) {

  // Check for transparent colour
  if (is_transparent(colour)) return;

  // Flip y axis. matrices are (1, 1) at top left, we want nara plotted to be
  // (1, 1) at bottom-left
  y = height - y;
  x = x - 1;

  // Alpha channel for blending colours
  int alpha = (colour >> 24) & 255;

  if (y >= 0 && y < height && x >= 0 && x < width) {
    if (op == OP_INDEXED) {
      nr[y * width + x] = colour;
    } else if (op == OP_DRAW) {
      if (alpha == 255) {
        nr[y * width + x] = colour;
      } else {
        // Alpha blending
        int val = nr[y * width + x];
        unsigned char r2 =  val        & 255;
        unsigned char g2 = (val >>  8) & 255;
        unsigned char b2 = (val >> 16) & 255;
        // unsigned char a2 = (val >> 24) & 255;

        unsigned char r =  colour        & 255;
        unsigned char g = (colour >>  8) & 255;
        unsigned char b = (colour >> 16) & 255;
        // unsigned char a = (colour >> 24) & 255;

        r = (alpha * r + (255 - alpha) * r2) / 255;
        g = (alpha * g + (255 - alpha) * g2) / 255;
        b = (alpha * b + (255 - alpha) * b2) / 255;
        // a = (alpha * a + (255 - alpha) * a2) / 255;

        nr[y * width + x] = (r) | (g << 8) | (b << 16) | (255 << 24);
      }
    } else if (op == OP_OR) {
      nr[y * width + x] |= colour;
    }
  }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw multiple points on the canvas [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void draw_points_c(int *nr, unsigned int height, unsigned int width, int colour, int *x, int *y, int npoints, int op) {
  for (int i = 0 ; i < npoints; i++) {
    draw_point_c(nr, height, width, colour, x[i], y[i], op);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw points [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP draw_points_(SEXP nr_, SEXP colour_, SEXP x_, SEXP y_, SEXP op_) {

  assert_nativeraster(nr_);

  if (length(x_) != length(y_)) {
    error("x and y must be same length");
  }

  int *nr = INTEGER(nr_);

  SEXP dim = PROTECT(GET_DIM(nr_));
  unsigned int height = (unsigned int)INTEGER(dim)[0];
  unsigned int width  = (unsigned int)INTEGER(dim)[1];
  UNPROTECT(1);

  int op     = asInteger(op_);

  // get an int* from a numeric from R
  int freex = 0, freey = 0;
  int *x = dbl_to_int(x_, &freex);
  int *y = dbl_to_int(y_, &freey);

  int npoints = length(x_);

  if (!(length(colour_) == 1 || length(colour_) == length(x_))) {
    error("'colour' must be same length as 'x' or length=1");
  }

  int single_colour = length(colour_) == 1;
  int colour = colour_to_integer(colour_);

  for (int i = 0 ; i < npoints; i++) {
    colour = single_colour ? colour : colour_to_integer(STRING_ELT(colour_, i));
    draw_point_c(nr, height, width, colour, x[i], y[i], op);
  }


  // free and return
  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}






//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw line [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void draw_line_c(int *nr, unsigned int height, unsigned int width, int colour, int x0, int y0, int x1, int y1, int op) {

  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = dx+dy, e2;                                  /* error value e_xy */

  for (;;) {                                                        /* loop */
    draw_point_c(nr, height, width, colour, x0, y0, op);

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
SEXP draw_line_(SEXP nr_, SEXP colour_, SEXP x0_, SEXP y0_, SEXP x1_, SEXP y1_, SEXP op_) {

  assert_nativeraster(nr_);

  int *nr = INTEGER(nr_);

  SEXP dim = PROTECT(GET_DIM(nr_));
  unsigned int height = (unsigned int)INTEGER(dim)[0];
  unsigned int width  = (unsigned int)INTEGER(dim)[1];
  UNPROTECT(1);

  int colour = colour_to_integer(colour_);
  int op = asInteger(op_);

  if (!(length(x0_) == length(x1_) &&
    length(x0_) == length(y0_) &&
    length(x0_) == length(y1_))) {
    error("All arguments must be same length x0,y0,x1,y1");
  }

  // get an int* from a numeric from R
  int freex0 = 0, freey0 = 0, freex1 = 0, freey1 = 0;
  int *x0 = dbl_to_int(x0_, &freex0);
  int *y0 = dbl_to_int(y0_, &freey0);
  int *x1 = dbl_to_int(x1_, &freex1);
  int *y1 = dbl_to_int(y1_, &freey1);

  int single_colour = length(colour_) == 1;
  if (!(length(colour_) == length(x0_) || length(colour_) == 1)) {
    error("'colour' must be same length as 'x' or length=1");
  }

  for (int i = 0; i < length(x0_); i++) {
    colour = single_colour ? colour : colour_to_integer(STRING_ELT(colour_, i));
    draw_line_c(nr, height, width, colour, x0[i], y0[i], x1[i], y1[i], op);
  }

  // free and return
  if (freex0) free(x0);
  if (freey0) free(y0);
  if (freex1) free(x1);
  if (freey1) free(y1);
  return nr_;
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw polyline [R interace]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP draw_polyline_(SEXP nr_, SEXP colour_, SEXP x_, SEXP y_, SEXP op_, SEXP close_) {

  assert_nativeraster(nr_);

  int *nr = INTEGER(nr_);

  SEXP dim = PROTECT(GET_DIM(nr_));
  unsigned int height = (unsigned int)INTEGER(dim)[0];
  unsigned int width  = (unsigned int)INTEGER(dim)[1];
  UNPROTECT(1);

  int colour = colour_to_integer(colour_);
  int op = asInteger(op_);

  // get an int* from a numeric from R
  int freex = 0, freey = 0;
  int *x = dbl_to_int(x_, &freex);
  int *y = dbl_to_int(y_, &freey);

  if (length(x_) != length(y_)) {
    error("Arguments 'x' and 'y' must be same length.");
  }

  // Draw lines between pairs of points
  for (int i = 0; i < length(x_) - 1; i++) {
    draw_line_c(nr, height, width, colour, x[i], y[i], x[i+1], y[i+1], op);
  }

  if (asInteger(close_)) {
    // Join last point and first point if requested
    int n = length(x_);
    draw_line_c(nr, height, width, colour, x[n-1], y[n-1], x[0], y[0], op);
  }


  // free and return
  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw polygon [C interface]
// public domain code from
// https://www.alienryderflex.com/polygon_fill/
// not as efficient as something with an active edge table but it
// get me 30fps in "Another World" so I'm moving on.  Patches/PR welcomed!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void fill_polygon_c(int *nr, int height, int width, int colour, int *x, int *y, int npoints, int op) {

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
          draw_point_c(nr, height, width,  colour, pixelX, pixelY, op);
        }
      }
    }
  }
  free(nodeX);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// R Polygon [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP draw_polygon_(SEXP nr_, SEXP x_, SEXP y_, SEXP fill_, SEXP colour_, SEXP op_) {

  assert_nativeraster(nr_);

  int *nr = INTEGER(nr_);

  SEXP dim = PROTECT(GET_DIM(nr_));
  unsigned int height = (unsigned int)INTEGER(dim)[0];
  unsigned int width  = (unsigned int)INTEGER(dim)[1];
  UNPROTECT(1);

  int colour = colour_to_integer(colour_);
  int fill   = colour_to_integer(fill_);
  int op = asInteger(op_);

  if (length(x_) != length(y_)) {
    error("Arguments 'x' and 'y' must be same length.");
  }

  // get an int* from a numeric from R
  int freex = 0, freey = 0;
  int *x = dbl_to_int(x_, &freex);
  int *y = dbl_to_int(y_, &freey);

  // Rprintf("Polygon Fill: %i\n", fill);
  fill_polygon_c(nr, height, width, fill, x, y, length(x_), op);

  // outline
  if (!is_transparent(colour)) {
    // Rprintf("Polygon Colour: %i\n", colour);
    draw_polyline_(nr_, colour_, x_, y_, op_, ScalarLogical(1));
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
SEXP draw_text_(SEXP nr_, SEXP str_, SEXP colour_, SEXP x_, SEXP y_, SEXP fontsize_) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack args
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  int *nr = INTEGER(nr_);

  SEXP nr_dim_ = PROTECT(GET_DIM(nr_));
  int  nr_height = INTEGER(nr_dim_)[0];
  int  nr_width  = INTEGER(nr_dim_)[1];
  UNPROTECT(1);

  int colour = colour_to_integer(colour_);
  int x = asInteger(x_);
  int y = asInteger(y_);

  const char *str = CHAR(asChar(str_));


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Choose font
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int fontsize = asInteger(fontsize_);
  unsigned char *font = spleen5x8;
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
      error("draw_text() only accepts plain ASCII.  Char out of range: %i = %s", c, str[char_idx]);
    }

    unsigned char *letter = &font[c * char_h];
    for (int row = 0; row < char_h; row ++) {
      for (int i = 0; i < char_w; i++) {
        if (letter[row] & (1 << (8 - i))) {
          draw_point_c(nr, nr_height, nr_width, colour, col + i + x, y + (char_h - 1) - row, OP_DRAW);
        }
      }
    }

    col += char_w;
  }

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

  SEXP nr_dim_ = PROTECT(GET_DIM(nr_));
  int  nr_height = INTEGER(nr_dim_)[0];
  int  nr_width  = INTEGER(nr_dim_)[1];
  UNPROTECT(1);

  if (length(x_) != length(y_)) {
    error("'x' and 'y' must be the same length");
  }

  if (!(length(r_) == length(x_) || length(r_) == 1)) {
    error("'r' must have same length as 'x' or length=1");
  }

  int freexms = 0, freeyms = 0, freers = 0;
  int *xms = dbl_to_int(x_, &freexms);
  int *yms = dbl_to_int(y_, &freeyms);
  int *rs  = dbl_to_int(r_, &freers);

  int r = rs[0];
  int single_r = length(r_) == 1;

  int colour = colour_to_integer(colour_);
  int fill   = colour_to_integer(fill_);
  int single_colour = length(colour_) == 1;
  int single_fill   = length(fill_)   == 1;

  for (int idx = 0; idx < length(x_); idx++) {
    // Rprintf("idx: %i\n", idx);
    int xm = xms[idx];
    int ym = yms[idx];
    r = single_r ? rs[0] : rs[idx];
    colour = single_colour ? colour : colour_to_integer(STRING_ELT(colour_, idx));
    fill   = single_fill   ? fill   : colour_to_integer(STRING_ELT(fill_  , idx));

    int x = -r, y = 0, err = 2-2*r; /* II. Quadrant */
    do {

      for (int i = xm + x; i <= xm - x; i++) {
        draw_point_c(nr, nr_height, nr_width, fill, i, ym + y, OP_DRAW);
        draw_point_c(nr, nr_height, nr_width, fill, i, ym - y, OP_DRAW);
      }

      draw_point_c(nr, nr_height, nr_width, colour, xm-x, ym+y, OP_DRAW); /*   I. Quadrant */
      draw_point_c(nr, nr_height, nr_width, colour, xm+x, ym+y, OP_DRAW); /*  II. Quadrant */
      draw_point_c(nr, nr_height, nr_width, colour, xm-x, ym-y, OP_DRAW); /* III. Quadrant */
      draw_point_c(nr, nr_height, nr_width, colour, xm+x, ym-y, OP_DRAW); /*  IV. Quadrant */

      r = err;
      if (r <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
      if (r > x || err > y) err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
    } while (x < 0);
  }


  if (freexms) free(xms);
  if (freeyms) free(yms);
  if (freers)  free(rs);
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

  SEXP nr_dim_ = PROTECT(GET_DIM(nr_));
  int  nr_height = INTEGER(nr_dim_)[0];
  int  nr_width  = INTEGER(nr_dim_)[1];
  UNPROTECT(1);

  if (length(x_) != length(y_)) {
    error("Arguments 'x' and 'y' must be same length.");
  }
  int N = length(x_);
  if (!(length(h_) == N || length(h_) == 1)) {
    error("'h' must be same length as 'x' or length=1");
  }
  if (!(length(w_) == N || length(w_) == 1)) {
    error("'w' must be same length as 'x' or length=1");
  }
  int single_h = length(h_) == 1;
  int single_w = length(w_) == 1;


  int freex = 0, freey = 0, freew = 0, freeh = 0;
  int *xs = dbl_to_int(x_, &freex);
  int *ys = dbl_to_int(y_, &freey);
  int *ws = dbl_to_int(w_, &freew);
  int *hs = dbl_to_int(h_, &freeh);

  int w = ws[0] - 1;
  int h = ws[0] - 1;


  int colour = colour_to_integer(colour_);
  int fill   = colour_to_integer(fill_);
  int single_colour = length(colour_) == 1;
  int single_fill   = length(fill_)   == 1;

  for (int i = 0; i < length(x_); i++) {

    int x = xs[i];
    int y = ys[i];
    w = single_w ? w : ws[i] - 1;
    h = single_h ? h : hs[i] - 1;

    colour = single_colour ? colour : colour_to_integer(STRING_ELT(colour_, i));
    fill   = single_fill   ? fill   : colour_to_integer(STRING_ELT(fill_  , i));

    // Draw Filled rect
    if (!is_transparent(fill)) {
      for (int row = y; row <= y + h; row++) {
        for (int col = x; col <= x + w; col++) {
          draw_point_c(nr, nr_height, nr_width,  fill, col, row, OP_DRAW);
        }
      }
    }

    // Draw outline
    if (!is_transparent(colour)) {
      draw_line_c(nr, nr_height, nr_width, colour, x  , y  , x+w, y  , OP_DRAW);
      draw_line_c(nr, nr_height, nr_width, colour, x+w, y  , x+w, y+h, OP_DRAW);
      draw_line_c(nr, nr_height, nr_width, colour, x+w, y+h, x  , y+h, OP_DRAW);
      draw_line_c(nr, nr_height, nr_width, colour, x  , y+h, x  , y  , OP_DRAW);
    }
  }


  if (freex) free(xs);
  if (freey) free(ys);
  if (freew) free(ws);
  if (freeh) free(hs);
  return nr_;
}





