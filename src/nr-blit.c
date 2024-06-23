


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
      x + w < 0 ||
        y + h < 0 ||
        x >= dst_width ||
        y >= dst_height
  ) {
    return;
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Some of the src overlaps the dst
  // Trim src boundaries if they're outside the image
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (x < 0) {
    // Trim left of src
    int diff = -x;
    x0 += diff;
    w  -= diff ;
    x   = 0;
  }
  if (y < 0) {
    // Trim bottom of src
    int diff = -y ;
    y0 += diff;
    h  -= diff ;
    y   = 0;
  }
  if (x + w >= dst_width) {
    // Trim right of src
    w = dst_width - x;
  }
  if (y + h >= dst_height) {
    // Trim right of src
    h = dst_height - y;
  }
  
  if (w == 0 || h == 0) return;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If 'respect_alpha', then copy pixel by pixel
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (respect_alpha) {
    for (int yoff = 0; yoff < h; yoff++) {
      int y1 = y0 + yoff ;
      for (int xoff = 0; xoff < w; xoff++) {
        uint32_t src_col = src[y1 * src_width + x0 + xoff];
        draw_point_c(dst, dst_height, dst_width, src_col, x + xoff, y + yoff);
      }
    }
  } else {
    // Blit it via 'memcpy()' trashing any contents
    // Flip y axis. matrices are (0, 0) at top left,
    for (int yoff = 0; yoff < h; yoff++) {
      int y1 = y0 + yoff;
      memcpy(
        dst + (y + yoff) * dst_width + x, 
        src + y1 * src_width + x0, 
        (unsigned long)w * sizeof(int32_t)
      );
    }
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit sprite onto raster [C interface]
// Slow and study, but it works
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
           SEXP hjust_, SEXP vjust_, SEXP respect_alpha_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity checks
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  assert_nativeraster(src_);
  
  if (!(length(x_) == 1 || length(y_) == 1 || length(x_) == length(y_))) {
    error("'x' and 'y' must be the length 1 or N.  x = %i, y = %i", length(x_), length(y_));
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Destination dimensions
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
  // Single native raster
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int x0 = asInteger(x0_);
  int y0 = asInteger(y0_);
  
  int src_width  = Rf_ncols(src_);
  int src_height = Rf_nrows(src_);
  
  int w = asInteger(w_) < 0 ? src_width  : asInteger(w_);
  int h = asInteger(h_) < 0 ? src_height : asInteger(h_);
  
  if (x0 + w - 1 >= src_width || y0 + h - 1 >= src_height) {
    error("Specified src [x0 = %i, y0 = %i] + [w = %i, h = %i] is outside bounds [%i, %i]",
          x0, y0, w, h, src_width, src_height);
  }
  
  int hjust = (int)round(asReal(hjust_) * src_width);
  int vjust = (int)round(asReal(vjust_) * src_height);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Blit mulitple copies
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint32_t *nr  = (uint32_t *)INTEGER(nr_);
  uint32_t *src = (uint32_t *)INTEGER(src_);
  bool respect_alpha = asLogical(respect_alpha_);
  for (int i = 0; i < length(x_); i++) {
    blit_core_(nr, x[i] - hjust, y[i] - vjust, nr_width, nr_height, src, x0, y0, w, h, src_width, src_height, respect_alpha);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Tidy and return
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}


SEXP blit_list_(SEXP nr_, SEXP x_, SEXP y_, SEXP src_list_, SEXP src_idx_, SEXP hjust_, SEXP vjust_, SEXP respect_alpha_) {
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity checks
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  
  if (!isNewList(src_list_)) {
    error("nr_blit_list_(): src_list_ must be a list");
  }
  
  if (!(length(x_) == 1 || length(y_) == 1 || length(x_) == length(y_))) {
    error("'x' and 'y' must be the length 1 or N.  x = %i, y = %i", length(x_), length(y_));
  }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Destination dimensions
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
  
  bool freeindices = false, free_respect_alpha = false;
  int *indices = as_int32_vec(src_idx_, N, &freeindices);
  int *respect_alpha = as_int32_vec(respect_alpha_, N, &free_respect_alpha);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // List of native rasters
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool freehjust = false, freevjust = false;
  double *hjust = as_double_vec(hjust_, N, &freehjust);
  double *vjust = as_double_vec(vjust_, N, &freevjust);
  
  uint32_t *nr  = (uint32_t *)INTEGER(nr_);
  
  for (int i = 0; i < N; i++) {
    int idx = indices[i] - 1;
    if (idx >= length(src_list_) || idx < 0) {
      Rprintf("nr_blit_list_(): 'src_idx' is out of bounds for given 'src_list'. %i is not in range [0, %i]\n", idx, length(src_list_) - 1);
      continue;
    }
    SEXP src_ = VECTOR_ELT(src_list_, idx);
    assert_nativeraster(src_);
    uint32_t *src = (uint32_t *)INTEGER(src_);

    int src_width  = Rf_ncols(src_);
    int src_height = Rf_nrows(src_);

    int this_hjust = (int)round(hjust[i] * src_width);
    int this_vjust = (int)round(vjust[i] * src_height);

    blit_core_(nr, x[i] - this_hjust, y[i] - this_vjust, nr_width, nr_height,
               src, 0, 0, src_width, src_height, src_width, src_height,
               (bool)respect_alpha[i]);
    
    
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Tidy and return
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (freeindices) free(indices);
  if (free_respect_alpha) free(respect_alpha);   
  if (freehjust) free(hjust); 
  if (freevjust) free(vjust);   
  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}






