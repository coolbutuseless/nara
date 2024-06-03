


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
// Blit sprite onto raster [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void blit_core_(uint32_t *dst, int x, int y, int dst_width, int dst_height, 
                uint32_t *src, int x0, int y0, int w, int h, int src_width, int src_height,
                bool respect_alpha) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Check if src doesn't overlap with dst **AT ALL**
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (
      x + w < 1 ||
        y + h < 1 ||
        x > dst_width ||
        y > dst_height
  ) {
    // Rprintf(">>>>>>>> Bail\n");
    return;
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Some of the src overlaps the dst
  // Trim src boundaries if they're outside the image
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (x < 1) {
    // Trim left of src
    int diff = -x + 1;
    x0 += diff;
    w  -= diff ;
    x   = 1;
  }
  if (y < 1) {
    // Trim bottom of src
    int diff = -y + 1;
    y0 += diff;
    h  -= diff ;
    y   = 1;
  }
  if (x + w > dst_width) {
    // Trim right of src
    w = dst_width - x + 1;
  }
  if (y + h > dst_height) {
    // Trim right of src
    h = dst_height - y + 1;
  }
  
  if (w == 0 || h == 0) return;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If 'respect_alpha', then copy pixel by pixel
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (respect_alpha) {
    for (int yoff = 0; yoff < h; yoff++) {
      int y1 = y0 + yoff;
      for (int xoff = 0; xoff < w; xoff++) {
        uint32_t src_col = src[y1 * src_width + x0 + xoff - 1];
        draw_point_c(dst, dst_height, dst_width, src_col, x + xoff, y + yoff);
      }
    }
  } else {
    // Blit it via 'memcpy()' trashing any contents
    // Flip y axis. matrices are (1, 1) at top left,
    for (int yoff = 0; yoff < h; yoff++) {
      int y1 = y0 + yoff;
      memcpy(dst + (dst_height - y - yoff) * dst_width + x - 1, src + y1 * src_width + x0, w * sizeof(int32_t));
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprite onto raster [C interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void blit_core_naive_(uint32_t *dst, int x, int y, int dst_width, int dst_height, uint32_t *src, int x0, int y0, int w, int h, int src_width, int src_height) {
  
  for (int yoff = 0; yoff < h; yoff++) {
    int y1 = src_height - y0 - yoff;
    for (int xoff = 0; xoff < w; xoff++) {
      
      uint32_t src_col = src[y1 * src_width + x0 + xoff - 1];
      
      draw_point_c(dst, dst_height, dst_width, src_col, x + xoff, y + yoff);
    }
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprites into raster [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP blit_(SEXP nr_, SEXP x_, SEXP y_, SEXP src_, SEXP x0_, SEXP y0_, SEXP w_, SEXP h_,
           SEXP respect_alpha_) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  assert_nativeraster(src_);

  if (length(x_) != length(y_)) {
    error("'x' and 'y' must be the same length.");
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Destination deimensions
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int nr_width  = Rf_ncols(nr_);
  int nr_height = Rf_nrows(nr_);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Ensure the coordinates are integers
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool freex = false, freey = false;
  int N = calc_max_length(2, x_, y_);
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check the sprite dimensions
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int x0 = asInteger(x0_);
  int y0 = asInteger(y0_);
  
  int src_width  = Rf_ncols(src_);
  int src_height = Rf_nrows(src_);

  int w = isNull(w_) ? src_width : asInteger(w_);
  int h = isNull(h_) ? src_height : asInteger(h_);
  
  if (x0 + w - 1 > src_width || y0 + h - 1 > src_height) {
    error("Specified src [x0,y0] + [w,h] is outside bounds");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Blit mulitple copies
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint32_t *nr  = (uint32_t *)INTEGER(nr_);
  uint32_t *src = (uint32_t *)INTEGER(src_);
  bool respect_alpha = asLogical(respect_alpha_);
  for (int i = 0; i < length(x_); i++) {
    // blit_core_naive_(nr, x[i], y[i], nr_width, nr_height, src, x0, y0, w, h, src_width, src_height);
    blit_core_(nr, x[i], y[i], nr_width, nr_height, src, x0, y0, w, h, src_width, src_height, respect_alpha);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Tidy and return
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}






