
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Nearest neighbour resizing
//
// @param width,height desired output dimensions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP resize_nn_(SEXP nr_, SEXP width_, SEXP height_) {
  
  assert_nativeraster(nr_);
  
  // Get dims of src
  int in_width  = Rf_ncols(nr_);
  int in_height = Rf_nrows(nr_);
  
  int out_width  = Rf_asInteger(width_);
  int out_height = Rf_asInteger(height_);
  
  // Create nativeraster and copy contents
  SEXP dst_ = PROTECT(nr_new(out_width, out_height));

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Prepare set of row/column indices to fetch from
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int *src_col = malloc((unsigned long)out_width  * sizeof(int));
  int *src_row = malloc((unsigned long)out_height * sizeof(int));
  
  for (int i = 0; i < out_width; i++) {
    src_col[i] = (int)floor(i * in_width / out_width);
  }
  
  for (int i = 0; i < out_height; i++) {
    src_row[i] = (int)floor(i * in_height / out_height);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // 
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int *dst = INTEGER(dst_);
  int *src = INTEGER(nr_);
  
  for (int y = 0; y < out_height; y++) {
    for (int x = 0; x < out_width; x++) {
      dst[y * out_width + x] = src[src_row[y] * in_width + src_col[x]];
    }
  }
  
  free(src_col);
  free(src_row);
  UNPROTECT(1);
  return dst_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Linearly Interpolate between two RGBA colours
//
// @param first,second packed RGBA colours
// @param frac distance between two colours. Frac is between [0, 1]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t lerp(uint32_t first, uint32_t second, float frac) {
    // uint32_t val = 0;
    // uint8_t *vp = (uint8_t *)&val;
    // uint8_t *fp = (uint8_t *)&first;
    // uint8_t *sp = (uint8_t *)&second;
    // for (int i = 0; i < 4; i++) {
    //   vp[i] = (uint8_t)((double)fp[i] + frac * (double)(sp[i] - fp[i]));
    // }
    
    const uint8_t f0 = (first  >>  0) & 0xFF;
    const uint8_t f1 = (first  >>  8) & 0xFF;
    const uint8_t f2 = (first  >> 16) & 0xFF;
    const uint8_t f3 = (first  >> 24) & 0xFF;
    const uint8_t s0 = (second >>  0) & 0xFF;
    const uint8_t s1 = (second >>  8) & 0xFF;
    const uint8_t s2 = (second >> 16) & 0xFF;
    const uint8_t s3 = (second >> 24) & 0xFF;

    uint32_t val =
      (uint32_t)(f0 + frac * (s0 - f0)) <<  0 |
      (uint32_t)(f1 + frac * (s1 - f1)) <<  8 |
      (uint32_t)(f2 + frac * (s2 - f2)) << 16 |
      (uint32_t)(f3 + frac * (s3 - f3)) << 24;
    
    return val;    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Interpolate a given row
//
// @param in_row a pointer to the start of the row in the input native raster
// @param out_row a pointer to the start of the row in the output nattive raster
// @param out_width the width of the row of the output native raster
// @param left_col the sequence of left column to user the interpolation of
//        the output
// @param frac the fractional distance between this left column and the next
//        column to user for the interpolation
//
// @return nothing.  out_row should be populated with interpolated pixels
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void lerp_row(uint32_t *in_row, int in_width, uint32_t *out_row, int out_width, int *left_col, float *frac_col) {
  
  for (int i = 0; i < out_width; i++) {
    // Rprintf(">>>> %i/%i [%.3f]\n", left_col[i], in_width, frac_col[i]);
    out_row[i] = lerp(in_row[left_col[i]], in_row[left_col[i] + 1], frac_col[i]);
  }
  
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize with bilinear intepolation.
// This interpolates in RGBA space - which isn't great.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP resize_bilinear_(SEXP nr_, SEXP width_, SEXP height_) {
  
  assert_nativeraster(nr_);
  
  // Get dims of src
  int in_width  = Rf_ncols(nr_);
  int in_height = Rf_nrows(nr_);
  
  int out_width  = Rf_asInteger(width_);
  int out_height = Rf_asInteger(height_);
  
  if (out_width <= 2 || out_height <= 2) {
    Rf_error("resize_bilinear_(): new height/width must be 2 pixels or greater");
  }
  
  // Create nativeraster and copy contents
  SEXP dst_ = PROTECT(nr_new(out_width, out_height));
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Prepare set of row/column indices to fetch from
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int *left_col  = malloc((unsigned long)out_width  * sizeof(int));
  int *upper_row = malloc((unsigned long)out_height * sizeof(int));
  
  float *frac_col = malloc((unsigned long)out_width  * sizeof(float));
  float *frac_row = malloc((unsigned long)out_height * sizeof(float));
  
  if (left_col == NULL || upper_row == NULL || frac_col == NULL || frac_row == NULL) {
    Rf_error("resize_bilinear_(): Memory allocation error");
  }
  
  for (int i = 0; i < out_width; i++) {
    float col = (float)i * ((float)in_width - 1)/ (float)(out_width - 1);
    left_col[i] = (int)floor(col);
    frac_col[i] = col - (float)floor(col);
    if (left_col[i] == in_width - 1) {
      left_col[i] = in_width - 2;
      frac_col[i] = 1;
    }
  }
  
  for (int i = 0; i < out_height; i++) {
    float row = (float)i * (float)(in_height - 1)/ (float)(out_height - 1);
    upper_row[i] = (int)floor(row);
    frac_row[i]  = row - (float)floor(row);
    if (upper_row[i] == in_height - 1) {
      upper_row[i] = in_height - 2;
      frac_row[i] = 1;
    }
  }
  // Special case handling for last row in order to avoid a boundary check in 
  // the innner lerp_row()
  upper_row[out_height - 1] = in_height - 2;
  frac_row [out_height - 1] = 1;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Interpolate
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint32_t *dst = (uint32_t *)INTEGER(dst_);
  uint32_t *src = (uint32_t *)INTEGER(nr_);
  
  uint32_t *upper_row_cache = malloc((unsigned long)out_width * sizeof(uint32_t));
  uint32_t *lower_row_cache = malloc((unsigned long)out_width * sizeof(uint32_t));
  if (lower_row_cache == NULL || upper_row_cache == NULL) {
    Rf_error("resize_bilinear_(): Memory allocation error for row cache");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Prime the row caches
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  lerp_row(src + (upper_row[0] + 0) * in_width, in_width, upper_row_cache, out_width, left_col, frac_col);
  lerp_row(src + (upper_row[0] + 1) * in_width, in_width, lower_row_cache, out_width, left_col, frac_col);
  int prior_in_idx = upper_row[0];
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Iterate by rows
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  for (int y = 0; y < out_height; y++) {
    
    // Prepare caches of the interpolated rows
    int in_idx = upper_row[y];
    
    if (in_idx != prior_in_idx) {
      // Upper row for interpolation has changed
      if (in_idx == prior_in_idx + 1) {
        // Copy the lower row cache into the upper row cache and 
        // calculate a new lower_row_cache
        memcpy(upper_row_cache, lower_row_cache, (unsigned long)out_width * sizeof(uint32_t));
        lerp_row(src + (in_idx + 1) * in_width, in_width, lower_row_cache, out_width, left_col, frac_col);
      } else {
        // Recalculate new upper/lower row caches
        lerp_row(src + (in_idx + 0) * in_width, in_width, upper_row_cache, out_width, left_col, frac_col);
        lerp_row(src + (in_idx + 1) * in_width, in_width, lower_row_cache, out_width, left_col, frac_col);
      }
    }
    for (int x = 0; x < out_width; x++) {
      dst[y * out_width + x] = lerp(upper_row_cache[x], lower_row_cache[x], frac_row[y]);
    }
    prior_in_idx = in_idx;
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // tidy and return
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  free(upper_row_cache);
  free(lower_row_cache);
  free(left_col);
  free(upper_row);
  free(frac_col);
  free(frac_row);
  UNPROTECT(1);
  return dst_;
}





