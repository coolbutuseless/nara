
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
#include "nr-blit-rotozoom.h"


double rotx(double x, double y, double cost, double sint) { 
  return x * cost - y * sint;
}


double roty(double x, double y, double cost, double sint) { 
  return x * sint + y * cost;
}

#define RC_BLACK 0xFF000000


void nr_blit_rotozoom(uint32_t *dst, int dst_width, int dst_height, int x, int y,
                      uint32_t *src, int src_width, int src_height, 
                      double theta, double sf) {
  
  double cost = cos(theta);
  double sint = sin(theta);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Determine extents to rotate within
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double x1 = sf * fabs(rotx(src_width/2,  src_height/2, cost, sint) + 1);
  double x2 = sf * fabs(rotx(src_width/2, -src_height/2, cost, sint) + 1);
  int xmax = (int)ceil(x1 > x2 ? x1 : x2);
  
  double y1 = sf * fabs(roty(src_width/2,  src_height/2, cost, sint) + 1);
  double y2 = sf * fabs(roty(src_width/2, -src_height/2, cost, sint) + 1);
  int ymax = (int)ceil(y1 > y2 ? y1 : y2);
  
  xmax = xmax > dst_width /2 ? dst_width /2 : xmax;
  ymax = ymax > dst_height/2 ? dst_height/2 : ymax;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop over all possible dst locations (a rectangular region)
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double isf = 1/sf;
  for (int xi = -xmax; xi <= xmax; ++xi) {
    for (int yi = -ymax; yi <= ymax; ++yi) {
      double x0 = rotx(xi, yi, cost, sint);
      double y0 = roty(xi, yi, cost, sint);
      
      x0 = round( (x0 * isf) + src_width /2 );
      y0 = round( (y0 * isf) + src_height/2 );
      
      if (x0 >= 0 && y0 >= 0 && x0 < src_width && y0 < src_height) {
        uint32_t col = src[(int)(y0 * src_width + x0)];
        nr_point(dst, dst_width, dst_height, x + xi, y + yi, col);
      }
      
    }
  }
  
}


SEXP nr_blit_rotozoom_(SEXP dst_, SEXP src_, SEXP x_, SEXP y_, SEXP angle_, SEXP sf_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(dst_);
  assert_nativeraster(src_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack rasters
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  const int dst_width  = Rf_ncols(dst_);
  const int dst_height = Rf_nrows(dst_);
  const int src_width  = Rf_ncols(src_);
  const int src_height = Rf_nrows(src_);
  
  uint32_t *dst = (uint32_t *)INTEGER(dst_);
  uint32_t *src = (uint32_t *)INTEGER(src_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack args. Extend to vectors if required
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int N = calc_max_length(4 , x_, y_, angle_, sf_);
  
  bool freex = false, freey = false, freesf= false, freetheta = false;
  double *xs     = as_double_vec(x_    , N, &freex);
  double *ys     = as_double_vec(y_    , N, &freey);
  double *sfs    = as_double_vec(sf_   , N, &freesf);
  double *thetas = as_double_vec(angle_, N, &freetheta);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Precalculate cos(theta) sin(theta). Maybe not needed.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  for (int i = 0; i < N; i++) {
    nr_blit_rotozoom(dst, dst_width, dst_height, xs[i], ys[i], 
                     src, src_width, src_height, thetas[i], sfs[i]);
  }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Bounding rectangle for debugging
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // if (0) {
  //   nr_line(dst, dst_width, dst_height, x + xmax,  y - ymax, x + xmax, y + ymax, RC_BLACK);
  //   nr_line(dst, dst_width, dst_height, x + xmax,  y + ymax, x - xmax, y + ymax, RC_BLACK);
  //   nr_line(dst, dst_width, dst_height, x - xmax,  y + ymax, x - xmax, y - ymax, RC_BLACK);
  //   nr_line(dst, dst_width, dst_height, x - xmax,  y - ymax, x + xmax, y - ymax, RC_BLACK);
  // }
  
  if (freex) free(xs);
  if (freey) free(ys);
  if (freesf) free(sfs);
  if (freetheta) free(thetas);
  
  return dst_;
}























