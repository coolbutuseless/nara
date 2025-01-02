
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
#define RC_RED 0xFF0000FF


void nr_blit_rotozoom(uint32_t *dst, int dst_width, int dst_height, int x, int y,
                      uint32_t *src, int src_width, int src_height, int x0, int y0,
                      int w, int h,
                      double hjust, double vjust,
                      double theta, double sf, 
                      bool respect_alpha) {
  
  double cost = cos(theta);
  double sint = sin(theta);
  
  // Rprintf(">>> w/h %i/%i\n", w, h);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Determine extents to rotate within
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double xr =  w * (1 - hjust);
  double xl = -w *      hjust ; 
  double yt =  h *      vjust ;
  double yb = -h * (1 - vjust);
  
  // Rprintf(">>>> xr/xl %.1f/%1.f  yt/yb  %.1f/%.1f\n", xr, xl, yt, yb);
  
  double x1 = sf * rotx(xr,  yt, cost, sint);
  double x2 = sf * rotx(xr,  yb, cost, sint);
  double x3 = sf * rotx(xl,  yb, cost, sint);
  double x4 = sf * rotx(xl,  yt, cost, sint);
  // Rprintf(">>>> x:  %.1f %.1f %.1f %.1f\n", x1, x2, x3, x4);
  double xmax = x1;
  xmax = x2 > xmax ? x2 : xmax;
  xmax = x3 > xmax ? x3 : xmax;
  xmax = x4 > xmax ? x4 : xmax;
  double xmin = x1;
  xmin = x2 < xmin ? x2 : xmin;
  xmin = x3 < xmin ? x3 : xmin;
  xmin = x4 < xmin ? x4 : xmin;

  double y1 = sf * roty(xr,  yt, cost, sint);
  double y2 = sf * roty(xr,  yb, cost, sint);
  double y3 = sf * roty(xl,  yb, cost, sint);
  double y4 = sf * roty(xl,  yt, cost, sint);
  // Rprintf(">>>> y:  %.1f %.1f %.1f %.1f\n", y1, y2, y3, y4);
  double ymax = y1;
  ymax = y2 > ymax ? y2 : ymax;
  ymax = y3 > ymax ? y3 : ymax;
  ymax = y4 > ymax ? y4 : ymax;
  double ymin = y1;
  ymin = y2 < ymin ? y2 : ymin;
  ymin = y3 < ymin ? y3 : ymin;
  ymin = y4 < ymin ? y4 : ymin;
  
  // Add a bit of a buffer to account for rounding
  xmin -= sf;
  xmax += sf;
  ymin -= sf;
  ymax += sf;
  
  // Trim so that x+xmin, x+xmax y+ymin y+ymax is always within dst
  xmax = xmax + x >= dst_width  ? dst_width  - x - 1 : xmax;
  ymax = ymax + y >= dst_height ? dst_height - y - 1 : ymax;
  
  xmin = xmin + x < 0  ? -x : xmin;
  ymin = ymin + y < 0  ? -y : ymin;
  
  // Rprintf(">>> xmin/xmax %.1f/%.1f  ymin/ymax %.1f/%.1f\n", xmin, xmax, ymin, ymax);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop over all possible dst locations (a rectangular region)
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double isf = 1/sf;
  for (int xi = floor(xmin); xi <= ceil(xmax); ++xi) {
    for (int yi = floor(ymin); yi <= ceil(ymax); ++yi) {
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
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Bounding rectangle for debugging
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (0) {
    nr_line(dst, dst_width, dst_height, x + xmin,  y + ymax, x + xmax, y + ymax, RC_RED);
    nr_line(dst, dst_width, dst_height, x + xmax,  y + ymax, x + xmax, y + ymin, RC_RED);
    nr_line(dst, dst_width, dst_height, x + xmax,  y + ymin, x + xmin, y + ymin, RC_RED);
    nr_line(dst, dst_width, dst_height, x + xmin,  y + ymin, x + xmin, y + ymax, RC_RED);
  }
  
  
}


SEXP nr_blit_rotozoom_(SEXP dst_, SEXP x_, SEXP y_, 
                       SEXP src_, SEXP x0_, SEXP y0_, 
                       SEXP w_    , SEXP h_, 
                       SEXP hjust_, SEXP vjust_, 
                       SEXP angle_, SEXP sf_,
                       SEXP respect_alpha_) {  
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
  // Only a Single native raster allowed as input.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int x0 = Rf_asInteger(x0_);
  int y0 = Rf_asInteger(y0_);
  
  int w = Rf_asInteger(w_) < 0 ? src_width  : Rf_asInteger(w_);
  int h = Rf_asInteger(h_) < 0 ? src_height : Rf_asInteger(h_);
    
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // For each of the locations in the destination, blit from the source
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  for (int i = 0; i < N; i++) {
    nr_blit_rotozoom(dst, dst_width, dst_height, xs[i], ys[i], 
                     src, src_width, src_height, x0   , y0, 
                     w, h,
                     Rf_asReal(hjust_), Rf_asReal(vjust_),
                     thetas[i], sfs[i], Rf_asLogical(respect_alpha_));
  }
  

  
  if (freex) free(xs);
  if (freey) free(ys);
  if (freesf) free(sfs);
  if (freetheta) free(thetas);
  
  return dst_;
}























