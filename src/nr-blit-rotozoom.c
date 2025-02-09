
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
                      uint32_t *src, int src_width, int src_height, int xsrc, int ysrc,
                      int w, int h,
                      double hjust, double vjust,
                      double angle, double scale, 
                      draw_mode_t draw_mode) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (xsrc < 0 || ysrc < 0 || 
      w < 0 || h < 0 || 
      xsrc + w > src_width || ysrc + h > src_height) {
    return;
  }

  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // pre-calculate trig values for transofrming forwards
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double cost = cos(-angle);
  double sint = sin(-angle);
  
  // Rprintf(">>> w/h %i/%i\n", w, h);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Determine extents to rotate within
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double xr =  w * (1 - hjust);
  double xl = -w *      hjust ; 
  double yt =  h * (1 - vjust);
  double yb = -h *      vjust ;
  
  // Rprintf(">>>> xr/xl %.1f/%1.f  yt/yb  %.1f/%.1f\n", xr, xl, yt, yb);
  
  double x1 = scale * rotx(xr,  yt, cost, sint);
  double x2 = scale * rotx(xr,  yb, cost, sint);
  double x3 = scale * rotx(xl,  yb, cost, sint);
  double x4 = scale * rotx(xl,  yt, cost, sint);
  // Rprintf(">>>> x:  %.1f %.1f %.1f %.1f\n", x1, x2, x3, x4);
  double xmax = x1;
  xmax = x2 > xmax ? x2 : xmax;
  xmax = x3 > xmax ? x3 : xmax;
  xmax = x4 > xmax ? x4 : xmax;
  double xmin = x1;
  xmin = x2 < xmin ? x2 : xmin;
  xmin = x3 < xmin ? x3 : xmin;
  xmin = x4 < xmin ? x4 : xmin;

  double y1 = scale * roty(xr,  yt, cost, sint);
  double y2 = scale * roty(xr,  yb, cost, sint);
  double y3 = scale * roty(xl,  yb, cost, sint);
  double y4 = scale * roty(xl,  yt, cost, sint);
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
  xmin -= scale;
  xmax += scale;
  ymin -= scale;
  ymax += scale;
  
  xmin = floor(xmin);
  xmax = ceil (xmax);
  ymin = floor(ymin);
  ymax = ceil (ymax);
  
  // Trim so that x+xmin, x+xmax y+ymin y+ymax is always within dst
  xmax = xmax + x >= dst_width  ? dst_width  - x - 1 : xmax;
  ymax = ymax + y >= dst_height ? dst_height - y - 1 : ymax;
  
  xmin = xmin + x < 0  ? -x : xmin;
  ymin = ymin + y < 0  ? -y : ymin;
  
  // Rprintf(">>> xmin/xmax %.1f/%.1f  ymin/ymax %.1f/%.1f\n", xmin, xmax, ymin, ymax);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // From here on, we're transforming backwards
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  cost = cos(angle);
  sint = sin(angle);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop over all possible dst locations (a rectangular region)
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double iscale = 1/scale;
  
  for (int xd = (int)xmin; xd <= (int)xmax; ++xd) {
    for (int yd = (int)ymin; yd <= (int)ymax; ++yd) {
      
      // (xd,yd) are the offset coordinates in the destination
      // (xs,ys) are the coordinates in the source
      double xs = rotx(xd, yd, cost, sint);
      double ys = roty(xd, yd, cost, sint);
      
      xs = round( (xs * iscale) + xsrc + w * hjust);
      ys = round( (ys * iscale) + ysrc + h * vjust);
      
      bool within_src = xs >= xsrc && ys >= ysrc && xs < xsrc + w && ys < ysrc + h;
      if (within_src) {
        uint32_t col = src[(int)(ys * src_width + xs)];
        if (draw_mode == RESPECT_ALPHA) {
          nr_point(dst, dst_width, dst_height, x + xd, y + yd, col, draw_mode);
        } else {
          // the range xmin/xmax and ymin/ymax are guarantted by the 
          // checks above to always be in the range of 'dst'
          dst[(int)((y + yd) * dst_width + (x + xd))] = col;
        }
      }
      
    }
  }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Bounding rectangle for debugging
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (0) {
    double linewidth = 1;
    draw_mode_t draw_mode = RESPECT_ALPHA;
    nr_line(dst, dst_width, dst_height, x + (int)xmin,  y + (int)ymax, x + (int)xmax, y + (int)ymax, RC_RED, linewidth, draw_mode);
    nr_line(dst, dst_width, dst_height, x + (int)xmax,  y + (int)ymax, x + (int)xmax, y + (int)ymin, RC_RED, linewidth, draw_mode);
    nr_line(dst, dst_width, dst_height, x + (int)xmax,  y + (int)ymin, x + (int)xmin, y + (int)ymin, RC_RED, linewidth, draw_mode);
    nr_line(dst, dst_width, dst_height, x + (int)xmin,  y + (int)ymin, x + (int)xmin, y + (int)ymax, RC_RED, linewidth, draw_mode);
  }
  
  
}


SEXP nr_blit_rotozoom_(SEXP dst_, SEXP x_, SEXP y_, 
                       SEXP src_, SEXP xsrc_, SEXP ysrc_, 
                       SEXP w_    , SEXP h_, 
                       SEXP hjust_, SEXP vjust_, 
                       SEXP angle_, SEXP scale_,
                       SEXP draw_mode_) {  
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
  int N = calc_max_length(4 , x_, y_, angle_, scale_);
  
  bool freex = false, freey = false, freescale= false, freeangle = false;
  int *xs        = as_int32_vec (x_    , N, &freex);
  int *ys        = as_int32_vec (y_    , N, &freey);
  double *scales    = as_double_vec(scale_   , N, &freescale);
  double *angles = as_double_vec(angle_, N, &freeangle);

  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Only a Single native raster allowed as input.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int xsrc = Rf_asInteger(xsrc_);
  int ysrc = Rf_asInteger(ysrc_);
  
  int w = Rf_asInteger(w_) < 0 ? src_width  : Rf_asInteger(w_);
  int h = Rf_asInteger(h_) < 0 ? src_height : Rf_asInteger(h_);
    
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // For each of the locations in the destination, blit from the source
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  for (int i = 0; i < N; i++) {
    nr_blit_rotozoom(dst, dst_width, dst_height, xs[i], ys[i], 
                     src, src_width, src_height, xsrc , ysrc, 
                     w, h,
                     Rf_asReal(hjust_), Rf_asReal(vjust_),
                     angles[i], scales[i], (draw_mode_t)Rf_asInteger(draw_mode_));
  }
  

  
  if (freex) free(xs);
  if (freey) free(ys);
  if (freescale) free(scales);
  if (freeangle) free(angles);
  
  return dst_;
}























