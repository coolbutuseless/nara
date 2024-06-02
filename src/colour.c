


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "colour.h"
#include "colour-from-r.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Check if the colour is fully transparent (so we can skip drawing)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool is_transparent(uint32_t colour) {
  return (colour & 0xff000000u) == 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert a hex digit to a nibble. 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static unsigned int hex_to_nibble(unsigned int x) {
  return (x & 0xf) + (x >> 6) + ((x >> 6) << 3);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Single colour as char * converted to packed colour
// Called from C
//
// @param colour R colour name or hex colour
// @return packed RGBA colour (integer)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t colour_char_to_packed_col(const char *colour) {
  uint32_t packed_col = 0;
  if (colour[0] == '#') {
    uint8_t *col = (uint8_t *)colour;
    switch (strlen(colour)) {
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
      error("Unknown hex colour: '%s'", colour);
    }
  } else {
    packed_col = rcolour_to_int(colour) ;
  }
  
  return packed_col;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Single colour as SEXP converted to packed colour
// Called from C
//
// @param colour Integer, CHARSXP, R colour name or hex colour
// @return packed RGBA colour (integer)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t colour_sexp_to_packed_col(SEXP colour_) {
  if (isInteger(colour_)) {
    return (uint32_t)asInteger(colour_);
  } else if (isString(colour_)) {
    return colour_char_to_packed_col(CHAR(asChar(colour_)));
  } else if (isLogical(colour_) && asLogical(colour_) == NA_LOGICAL) {
    return 0x00FFFFFF; // transparent white 0xAABBGGRR
  } else if (TYPEOF(colour_) == CHARSXP) {
    return colour_char_to_packed_col(CHAR(colour_));
  } else {
    error("Colour must be integer or character vector not '%s'", type2char((SEXPTYPE)TYPEOF(colour_)));
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SEXP vector of colours to a vector of packed colours
//
// @param colours_ Integer, logical or character vector of colours
// @param N the required length of colours
// @param do_free will be set to 'true' if the caller has to free the returned memory
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t *colours_to_packed_cols(SEXP colours_, int N, bool *do_free) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (length(colours_) != 1 && length(colours_) != N) {
    error("colours_to_packed_cols(): bad length %i/%i", length(colours_), N);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Fast-path requiring no allocation
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (isInteger(colours_) && length(colours_) == N) {
    *do_free = false;
    return (uint32_t *)INTEGER(colours_);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Allocate memory for an integer vector.
  // Set *do_free = 1 to notify the caller that they must free() returned ptr
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  *do_free = true;
  uint32_t *packed_cols = (uint32_t *)malloc((size_t)N * sizeof(uint32_t));
  if (packed_cols == NULL) {
    error("colours_to_packed_cols() malloc failed");
  }
  
  if (isLogical(colours_) && asLogical(colours_) == NA_LOGICAL) {
    uint32_t packed_col = 0x00FFFFFF; // transparent white 0xAABBGGRR
    for (int i = 0; i < N; i++) {
      packed_cols[i] = packed_col;
    }
  } else if (isInteger(colours_)) {
    // Must be length = 1 if we got here
    // Replicate same colour N times
    uint32_t packed_col = (uint32_t)asInteger(colours_);
    for (int i = 0; i < N; i++) {
      packed_cols[i] = packed_col;
    }
  } else if (isString(colours_)) {
    if (length(colours_) == 1) {
      uint32_t packed_col = colour_char_to_packed_col(CHAR(STRING_ELT(colours_, 0)));
      for (int i = 0; i < N; i++) {
        packed_cols[i] = packed_col;
      }
    } else {
      for (int i = 0; i < N; i++) {
        packed_cols[i] = colour_char_to_packed_col(CHAR(STRING_ELT(colours_, i)));
      }
    }
  } else {
    error("colours_to_packed_cols(): type fail '%s'", type2char((SEXPTYPE)TYPEOF(colours_)));
  }
  
  return packed_cols;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert single packed_col (integer) to a hexadecimal string in CHARSXP format
// Called from C
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP packed_col_to_CHARSXP_colour(uint32_t *packed_col) {
  
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
// Convert vector of packed colours into character vector of hex colours
// Called from R
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP packed_cols_to_hexcolours_(SEXP packed_cols_) {
  
  uint32_t *packed_col_ptr = (uint32_t *)INTEGER(packed_cols_);
  SEXP cols_ = PROTECT(allocVector(STRSXP, length(packed_cols_)));
  
  for (int i = 0; i < length(packed_cols_); i++) {
    SET_STRING_ELT(cols_, i, packed_col_to_CHARSXP_colour(packed_col_ptr + i));
  }
  
  UNPROTECT(1);
  return cols_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert colour names to packed colours
// Called from R
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP colours_to_packed_cols_(SEXP colours_) {
  
  if (isInteger(colours_)) {
    // Already integer. Do nothing
    return colours_;
  } 
  
  SEXP packed_cols_ = PROTECT(allocVector(INTSXP, length(colours_)));
  uint32_t *packed_cols = (uint32_t *)INTEGER(packed_cols_);
  
  if (isString(colours_)) {
    for (int i = 0; i < length(colours_); i++) {
      packed_cols[i] = colour_char_to_packed_col(CHAR(STRING_ELT(colours_, i)));
    }
  } else if (isLogical(colours_)) {
    // For when the user passes in logical NA
    for (int i = 0; i < length(colours_); i++) {
      packed_cols[i] = 0x00FFFFFF; // transparent white 0xAABBGGRR
    }
  } else {
    error("Colour must be integer or character vector not '%s'", type2char((SEXPTYPE)TYPEOF(colours_)));
  }
  
  UNPROTECT(1);
  return packed_cols_;
}

