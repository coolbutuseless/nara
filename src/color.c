


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "color.h"
#include "hash-color.h"

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
// Convert a hex digit to a nibble. 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static unsigned int hex_to_nibble(unsigned int x) {
  return (x & 0xf) + (x >> 6) + ((x >> 6) << 3);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Single color as char * converted to packed color
// Called from C
//
// @param color R color name or hex color
// @return packed RGBA color (integer)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t color_char_to_packed_col(const char *color) {
  uint32_t packed_col = 0;
  if (color[0] == '#') {
    uint8_t *col = (uint8_t *)color;
    switch (strlen(color)) {
    case 4:  // #123 == #112233
      packed_col =
        0xFF000000u + 
        (hex_to_nibble(col[3]) << 20) +
        (hex_to_nibble(col[3]) << 16) +
        (hex_to_nibble(col[2]) << 12) +
        (hex_to_nibble(col[2]) <<  8) +
        (hex_to_nibble(col[1]) <<  4) +
        (hex_to_nibble(col[1]) <<  0);
      break;
    case 5: // #1234 = #11223344
      packed_col =
        (hex_to_nibble(col[4]) << 28) +
        (hex_to_nibble(col[4]) << 24) +
        (hex_to_nibble(col[3]) << 20) +
        (hex_to_nibble(col[3]) << 16) +
        (hex_to_nibble(col[2]) << 12) +
        (hex_to_nibble(col[2]) <<  8) +
        (hex_to_nibble(col[1]) <<  4) +
        (hex_to_nibble(col[1]) <<  0);
      break;
    case 7: // #rrggbb
      packed_col =
        0xFF000000u + 
        (hex_to_nibble(col[5]) << 20) +
        (hex_to_nibble(col[6]) << 16) +
        (hex_to_nibble(col[3]) << 12) +
        (hex_to_nibble(col[4]) <<  8) +
        (hex_to_nibble(col[1]) <<  4) +
        (hex_to_nibble(col[2]) <<  0);
      break;
    case 9: // #rrggbbaa
      packed_col =
        (hex_to_nibble(col[7]) << 28) +
        (hex_to_nibble(col[8]) << 24) +
        (hex_to_nibble(col[5]) << 20) +
        (hex_to_nibble(col[6]) << 16) +
        (hex_to_nibble(col[3]) << 12) +
        (hex_to_nibble(col[4]) <<  8) +
        (hex_to_nibble(col[1]) <<  4) +
        (hex_to_nibble(col[2]) <<  0);
      break;
    default:
      error("Unknown hex color: '%s'", color);
    }
  } else {
    packed_col = rcolor_to_int(color) ;
  }
  
  return packed_col;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Single color as SEXP converted to packed color
// Called from C
//
// @param color Integer, CHARSXP, R color name or hex color
// @return packed RGBA color (integer)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t color_sexp_to_packed_col(SEXP color_) {
  if (isInteger(color_)) {
    return (uint32_t)asInteger(color_);
  } else if (isString(color_)) {
    return color_char_to_packed_col(CHAR(asChar(color_)));
  } else if (isLogical(color_) && asLogical(color_) == NA_LOGICAL) {
    return 0x00FFFFFF; // transparent white 0xAABBGGRR
  } else if (TYPEOF(color_) == CHARSXP) {
    return color_char_to_packed_col(CHAR(color_));
  } else {
    error("Color must be integer or character vector not '%s'", type2char((SEXPTYPE)TYPEOF(color_)));
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SEXP vector of colors to a vector of packed colors
//
// @param colors_ Integer, logical or character vector of colors
// @param N the required length of colors
// @param do_free will be set to 'true' if the caller has to free the returned memory
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t *colors_to_packed_cols(SEXP colors_, int N, bool *do_free) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (length(colors_) != 1 && length(colors_) != N) {
    error("colors_to_packed_cols(): bad length %i/%i", length(colors_), N);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Fast-path requiring no allocation
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (isInteger(colors_) && length(colors_) == N) {
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
    error("colors_to_packed_cols() malloc failed");
  }
  
  if (isLogical(colors_) && asLogical(colors_) == NA_LOGICAL) {
    uint32_t packed_col = 0x00FFFFFF; // transparent white 0xAABBGGRR
    for (int i = 0; i < N; i++) {
      packed_cols[i] = packed_col;
    }
  } else if (isInteger(colors_)) {
    // Must be length = 1 if we got here
    // Replicate same color N times
    uint32_t packed_col = (uint32_t)asInteger(colors_);
    for (int i = 0; i < N; i++) {
      packed_cols[i] = packed_col;
    }
  } else if (isString(colors_)) {
    if (length(colors_) == 1) {
      uint32_t packed_col = color_char_to_packed_col(CHAR(STRING_ELT(colors_, 0)));
      for (int i = 0; i < N; i++) {
        packed_cols[i] = packed_col;
      }
    } else {
      for (int i = 0; i < N; i++) {
        packed_cols[i] = color_char_to_packed_col(CHAR(STRING_ELT(colors_, i)));
      }
    }
  } else {
    error("colors_to_packed_cols(): type fail '%s'", type2char((SEXPTYPE)TYPEOF(colors_)));
  }
  
  return packed_cols;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert single packed_col (integer) to a hexadecimal string in CHARSXP format
// Called from C
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP packed_col_to_CHARSXP_color(uint32_t *packed_col) {
  
  static const char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  
  char buf[10];
  buf[0] = '#';
  buf[9] = '\0';
  
  uint8_t *bptr = (uint8_t *)packed_col;
  buf[1] = hexmap[(bptr[0] & 0xF0) >> 4];
  buf[2] = hexmap[(bptr[0] & 0x0F)     ];
  buf[3] = hexmap[(bptr[1] & 0xF0) >> 4];
  buf[4] = hexmap[(bptr[1] & 0x0F)     ];
  buf[5] = hexmap[(bptr[2] & 0xF0) >> 4];
  buf[6] = hexmap[(bptr[2] & 0x0F)     ];
  buf[7] = hexmap[(bptr[3] & 0xF0) >> 4];
  buf[8] = hexmap[(bptr[3] & 0x0F)     ];
  
  return mkChar(buf);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert vector of packed colors into character vector of hex colors
// Called from R
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP packed_cols_to_hexcolors_(SEXP packed_cols_) {
  
  uint32_t *packed_col_ptr = (uint32_t *)INTEGER(packed_cols_);
  SEXP cols_ = PROTECT(allocVector(STRSXP, length(packed_cols_)));
  
  for (int i = 0; i < length(packed_cols_); i++) {
    SET_STRING_ELT(cols_, i, packed_col_to_CHARSXP_color(packed_col_ptr + i));
  }
  
  UNPROTECT(1);
  return cols_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert color names to packed colors
// Called from R
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP colors_to_packed_cols_(SEXP colors_) {
  
  if (isInteger(colors_)) {
    // Already integer. Do nothing
    return colors_;
  } 
  
  SEXP packed_cols_ = PROTECT(allocVector(INTSXP, length(colors_)));
  uint32_t *packed_cols = (uint32_t *)INTEGER(packed_cols_);
  
  if (isString(colors_)) {
    for (int i = 0; i < length(colors_); i++) {
      packed_cols[i] = color_char_to_packed_col(CHAR(STRING_ELT(colors_, i)));
    }
  } else if (isLogical(colors_)) {
    // For when the user passes in logical NA
    for (int i = 0; i < length(colors_); i++) {
      packed_cols[i] = 0x00FFFFFF; // transparent white 0xAABBGGRR
    }
  } else {
    error("Color must be integer or character vector not '%s'", type2char((SEXPTYPE)TYPEOF(colors_)));
  }
  
  UNPROTECT(1);
  return packed_cols_;
}

