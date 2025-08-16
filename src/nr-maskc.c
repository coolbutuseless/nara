
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
// Mask begin
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_mask_begin_(SEXP nr_, SEXP mask_) {
  
  int nprotect = 0;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  assert_nativeraster(mask_);
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Check sizes are conformable
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if ((Rf_nrows(nr_) != Rf_nrows(mask_)) || 
      (Rf_ncols(nr_) != Rf_ncols(mask_))) {
    Rf_error("nr_mask_begin_(): 'mask' and 'nr' must have the same dimensions");
  }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Add the mask
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Rf_setAttrib(nr_, Rf_install("mask"), mask_);

  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Add a copy of the current state of 'nr'
  // Use a pre-allocated cache if available
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  SEXP cache_ = Rf_getAttrib(nr_, Rf_install("cache"));
  bool new_cache = false;
  if (Rf_isNull(cache_)) {
    new_cache = true;
    cache_ = PROTECT(Rf_duplicate(nr_)); nprotect++;
  }
  
  if (!new_cache) {
    if ((Rf_nrows(nr_) != Rf_nrows(cache_)) || 
        (Rf_ncols(nr_) != Rf_ncols(cache_))) {
      Rf_error("nr_mask_begin_(): 'cache' and 'nr' must have the same dimensions");
    }
    
    int width = Rf_ncols(nr_);
    int height = Rf_nrows(nr_);
    memcpy(INTEGER(cache_), INTEGER(nr_), (size_t)width * (size_t)height * sizeof(uint32_t));
  }
  
  
  Rf_setAttrib(nr_, Rf_install("cache"), cache_);
  
  
  UNPROTECT(nprotect);
  return nr_;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Mask end
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_mask_end_(SEXP nr_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Find if we have a mask
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  SEXP mask_ = Rf_getAttrib(nr_, Rf_install("mask"));
  if (Rf_isNull(mask_)) {
    Rf_error("nr_mask_end_() called, but no mask currently defined on 'nr'");
  }
  assert_nativeraster(mask_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Find if we have a cache
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  SEXP cache_ = Rf_getAttrib(nr_, Rf_install("cache"));
  if (Rf_isNull(cache_)) {
    Rf_error("nr_mask_end_() called, but no cache currently defined on 'nr'");
  }
  assert_nativeraster(cache_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Check sizes are conformable
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if ((Rf_nrows(nr_) != Rf_nrows(mask_)) || 
      (Rf_ncols(nr_) != Rf_ncols(mask_))) {
    Rf_error("nr_mask_end_(): 'mask' and 'nr' must have the same dimensions");
  }
  
  if ((Rf_nrows(nr_) != Rf_nrows(cache_)) || 
      (Rf_ncols(nr_) != Rf_ncols(cache_))) {
    Rf_error("nr_mask_end_(): 'cache' and 'nr' must have the same dimensions");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Do 
  //   - masked copy from 'nr' to 'cache' using 'mask'
  //   - copy 'cache' over 'nr'
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int width  = Rf_ncols(nr_);
  int height = Rf_nrows(nr_);
  
  uint32_t *nr    = (uint32_t *)INTEGER(nr_);
  uint32_t *mask  = (uint32_t *)INTEGER(mask_);
  uint32_t *cache = (uint32_t *)INTEGER(cache_);
  
  // Copy current 'nr' into 'cache' using the 'mask'
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      int loc = row * width + col;
      if (is_opaque(mask[loc]))
        cache[loc] = nr[loc];
    }
  }
  
  // Copy 'cache' back into 'nr'
  memcpy(nr, cache, (size_t)width * (size_t)height * sizeof(uint32_t));
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Remove mask attributes, but keep cache to re-use memory (save on allocations)
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Rf_setAttrib(nr_, Rf_install("mask" ), R_NilValue);
  
  
  return nr_;  
}











