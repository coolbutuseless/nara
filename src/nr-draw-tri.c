
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

#define FMT_WIDE 1
#define FMT_LONG 2

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// is (cx, cy) to the left of the line from (ax, ay) to (bx, by)?
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// double left_of(double ax, double ay, double bx, double by, double cx, double cy) {
//   return ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay)) >= 0;
// }

double det(double ax, double ay, double bx, double by, double cx, double cy) {
  return ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
}


double min3(double a, double b, double c) {
  double temp = (a < b)    ? a : b;
  return (c < temp) ? c : temp;
}


double max3(double a, double b, double c) {
  double temp = (a > b) ? a : b;
  return (c > temp) ? c : temp;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void draw_tri(uint32_t *nr, int nr_width, int nr_height, 
              double x0, double y0, 
              double x1, double y1, 
              double x2, double y2,
              int color, 
              draw_mode_t draw_mode) {
  
  // // Do nothing if triangle not oriented correctly
  bool tri_ok = det(x0, y0, x1, y1, x2, y2) >= 0;
  if (!tri_ok) return;

  // if (!tri_ok) {
  //   // swap 2 verts to make it ok
    // double tmp;
    // tmp = x0; x0 = x1; x1 = tmp;
    // tmp = y0; y0 = y1; y1 = tmp;
  // }
  
  
  double xmin = min3(x0, x1, x2);
  double xmax = max3(x0, x1, x2);
  double ymin = min3(y0, y1, y2);
  double ymax = max3(y0, y1, y2);
  
  xmin = xmin < 0 ? 0 : xmin;
  xmax = xmax > nr_width ? nr_width : xmax;
  ymin = ymin < 0 ? 0 : ymin;
  ymax = ymax > nr_height ? nr_height : ymax;
  
  for (int x = xmin; x < xmax; x++) {
    for (int y = ymin; y < ymax; y++) {
      if (det(x0, y0, x1, y1, x, y) >= 0 &&
          det(x1, y1, x2, y2, x, y) >= 0 &&
          det(x2, y2, x0, y0, x, y) >= 0) {
        nr_point(nr, nr_width, nr_height, x, y, color, draw_mode);
      } 
    }
  }
  
  
  // nr_line(nr, nr_width, nr_height, x0, y0, x1, y1, 0xFF000000, 1, draw_mode);
  // nr_line(nr, nr_width, nr_height, x1, y1, x2, y2, 0xFF000000, 1, draw_mode);
  // nr_line(nr, nr_width, nr_height, x2, y2, x0, y0, 0xFF000000, 1, draw_mode);
  // 
  // nr_point(nr, nr_width, nr_height, x0, y0, color, draw_mode);
  // nr_point(nr, nr_width, nr_height, x1, y1, color, draw_mode);
  // nr_point(nr, nr_width, nr_height, x2, y2, color, draw_mode);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Integer coordinates only
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_tri_coords_(SEXP nr_, SEXP coords_, SEXP color_, SEXP draw_all_, SEXP format_) {
  
  assert_nativeraster(nr_);
  
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  
  int nr_height = Rf_nrows(nr_);
  int nr_width  = Rf_ncols(nr_);
  
  if (TYPEOF(coords_) != REALSXP) {
    Rf_error("nr_tri_coords_(): 'coords' data must be numeric");
  }
  if (!Rf_isMatrix(coords_)) {
    Rf_error("nr_tri_coords_(): 'coords' data must a matrix");
  }
  
  int n_coords = Rf_ncols(coords_);
  int stride   = Rf_nrows(coords_);
  // int format   = FMT_WIDE;
  if (stride > n_coords) {
    Rf_error("nr_tri_coords_(): Long data not supported yet");
  }
  
  double *coords = REAL(coords_);
  int n_tris = n_coords / 3;

  bool free_color = false;
  uint32_t *color = multi_rcolors_to_ints(color_, n_tris, &free_color);
  
  // Rprintf("%i / %i\n", n_coords, n_tris);
  draw_mode_t draw_mode = 1; // respect alpha
  int col_idx = 0;
  for (int i = 0; i < n_coords * stride; i += 3 * stride) {
    // Rprintf("[%i] (%.2f, %.2f)\n", i, coords[i], coords[i + 1]);
    // nr_point(nr, nr_width, nr_height, coords[i], coords[i + 1], color[i], draw_mode);
    draw_tri(nr, nr_width, nr_height, 
             coords[stride * 0 + i], coords[stride * 0 + i + 1],
             coords[stride * 1 + i], coords[stride * 1 + i + 1],
             coords[stride * 2 + i], coords[stride * 2 + i + 1],
             color[col_idx], draw_mode); 
    col_idx++;
  }
  
  
  
  if (free_color) free(color);
  return nr_;
}

