
#define R_NO_REMAP

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include <colorfast.h>

#include "color.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Check if the color is fully transparent (so we can skip drawing)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool is_transparent(uint32_t color) {
  return (color & 0xff000000u) == 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Check if the color is fully transparent (so we can skip drawing)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool is_opaque(uint32_t color) {
  return (color >> 24) == 0xff;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Single color as SEXP converted to packed color
// Called from C
//
// @param color Integer, CHARSXP, R color name or hex color
// @return packed RGBA color (integer)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t single_rcolor_to_int(SEXP color_) {
  if (Rf_isInteger(color_)) {
    return (uint32_t)Rf_asInteger(color_);
  } else if (Rf_isString(color_)) {
    return col_to_int(CHAR(Rf_asChar(color_)));
  } else if (Rf_isLogical(color_) && Rf_asLogical(color_) == NA_LOGICAL) {
    return 0x00FFFFFF; // transparent white 0xAABBGGRR
  } else if (TYPEOF(color_) == CHARSXP) {
    return col_to_int(CHAR(color_)); // from {colorfast}
  } else {
    Rf_error("Color must be integer or character vector not '%s'", Rf_type2char((SEXPTYPE)TYPEOF(color_)));
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SEXP vector of colors to a vector of packed colors
//
// @param colors_ Integer, logical or character vector of colors
// @param N the required Rf_length of colors
// @param do_free will be set to 'true' if the caller has to free the returned memory
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t *multi_rcolors_to_ints(SEXP colors_, int N, bool *do_free) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (Rf_length(colors_) != 1 && Rf_length(colors_) != N) {
    Rf_error("multi_rcolors_to_ints(): bad Rf_length %i/%i", Rf_length(colors_), N);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Fast-path requiring no allocation
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (Rf_isInteger(colors_) && Rf_length(colors_) == N) {
    *do_free = false;
    return (uint32_t *)INTEGER(colors_);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Allocate memory for an integer vector.
  // Set *do_free = 1 to notify the caller that they must free() returned ptr
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  *do_free = true;
  uint32_t *packed_cols = (uint32_t *)malloc((size_t)N * sizeof(uint32_t));
  if (packed_cols == NULL) {
    Rf_error("multi_rcolors_to_ints() malloc failed");
  }
  
  if (Rf_isLogical(colors_)) {
    if (Rf_length(colors_) == 1) {
      if (Rf_asLogical(colors_) == NA_LOGICAL) {
        for (int i = 0; i < N; i++) {
          packed_cols[i] = 0x00FFFFFF;
        }
      } else {
        Rf_error("multi_rcolors_to_ints(): Invalid use of logical value as color");
      }
    } else {
      int *lgl = LOGICAL(colors_);
      for (int i = 0; i < N; i++) {
        if (lgl[i] == NA_LOGICAL) {
          packed_cols[i] = 0x00FFFFFF; // transparent white 0xAABBGGRR
        } else {
          Rf_error("multi_rcolors_to_ints(): Invalid use of logical value as color '%i'", lgl[i]);
        }
      }
    }
  } else if (Rf_isInteger(colors_)) {
    // Must be Rf_length = 1 if we got here
    // Replicate same color N times
    uint32_t packed_col = (uint32_t)Rf_asInteger(colors_);
    for (int i = 0; i < N; i++) {
      packed_cols[i] = packed_col;
    }
  } else if (Rf_isString(colors_)) {
    if (Rf_length(colors_) == 1) {
      uint32_t packed_col = col_to_int(CHAR(STRING_ELT(colors_, 0)));
      for (int i = 0; i < N; i++) {
        packed_cols[i] = packed_col;
      }
    } else {
      for (int i = 0; i < N; i++) {
        packed_cols[i] = col_to_int(CHAR(STRING_ELT(colors_, i)));
      }
    }
  } else {
    Rf_error("multi_rcolors_to_ints(): type fail '%s'", Rf_type2char((SEXPTYPE)TYPEOF(colors_)));
  }
  
  return packed_cols;
}

