


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "colour.h"
#include "colour-from-r.h"

// Reverse byte ordering in an int
#define reverse_bytes_32(num) ( ((num & 0xFF000000) >> 24) | ((num & 0x00FF0000) >> 8) | ((num & 0x0000FF00) << 8) | ((num & 0x000000FF) << 24) )


int is_transparent(int colour) {
  return ((colour >> 24) & 255) == 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert a hex digit to a nibble. Return -1 if not a hexdigits
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static unsigned int hex_to_nibble(int digit) {
  if('0' <= digit && digit <= '9') return      digit - '0';
  if('A' <= digit && digit <= 'F') return 10 + digit - 'A';
  if('a' <= digit && digit <= 'f') return 10 + digit - 'a';
  error("Invalid hex: '%c'", digit);
  return (int)digit; // silence return warnings
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Single character colour to packed-integer-colour
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int single_str_col_to_int(const char *colour) {
  if (colour[0] == '#') {
    switch (strlen(colour)) {
    case 3:
      return
        (0xff << 24) + 
        (hex_to_nibble(colour[3]) << 20) +
        (hex_to_nibble(colour[3]) << 16) +
        (hex_to_nibble(colour[2]) << 12) +
        (hex_to_nibble(colour[2]) <<  8) +
        (hex_to_nibble(colour[1]) <<  4) +
        (hex_to_nibble(colour[1]) <<  0);
    case 4:
      return
        (hex_to_nibble(colour[4]) << 28) +
        (hex_to_nibble(colour[4]) << 24) +
        (hex_to_nibble(colour[3]) << 20) +
        (hex_to_nibble(colour[3]) << 16) +
        (hex_to_nibble(colour[2]) << 12) +
        (hex_to_nibble(colour[2]) <<  8) +
        (hex_to_nibble(colour[1]) <<  4) +
        (hex_to_nibble(colour[1]) <<  0);
    case 7:
      return
        (0xff << 24) + 
        (hex_to_nibble(colour[5]) << 20) +
        (hex_to_nibble(colour[6]) << 16) +
        (hex_to_nibble(colour[3]) << 12) +
        (hex_to_nibble(colour[4]) <<  8) +
        (hex_to_nibble(colour[1]) <<  4) +
        (hex_to_nibble(colour[2]) <<  0);
    case 9:
      return
        (hex_to_nibble(colour[7]) << 28) +
        (hex_to_nibble(colour[8]) << 24) +
        (hex_to_nibble(colour[5]) << 20) +
        (hex_to_nibble(colour[6]) << 16) +
        (hex_to_nibble(colour[3]) << 12) +
        (hex_to_nibble(colour[4]) <<  8) +
        (hex_to_nibble(colour[1]) <<  4) +
        (hex_to_nibble(colour[2]) <<  0);
    default:
      error("Unknown hex colour: '%s'", colour);
    }
  } else {
    return rcolour_to_int(colour) ;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Single SEXP colour to packed-integer-colour
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int single_sexp_col_to_int(SEXP colour_) {
  if (isInteger(colour_)) {
    return asInteger(colour_);
  } else if (isString(colour_)) {
    return single_str_col_to_int(CHAR(asChar(colour_)));
  } else if (isLogical(colour_) && asLogical(colour_) == NA_LOGICAL) {
    return 16777215; // #ffffff00 transparent white
  } else if (TYPEOF(colour_) == CHARSXP) {
    return single_str_col_to_int(CHAR(colour_));
  } else {
    error("Colour must be integer or character vector not '%s'", type2char(TYPEOF(colour_)));
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SEXP vector of colours to packed-integer-colour
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP col_to_int_(SEXP colour_) {
  
  if (isInteger(colour_)) {
    return colour_;
  } else if (isString(colour_)) {
    SEXP res_ = PROTECT(allocVector(INTSXP, length(colour_)));
    int *ptr = INTEGER(res_);
    for (int i = 0; i < length(colour_); i++) {
      ptr[i] = single_str_col_to_int(CHAR(STRING_ELT(colour_, i)));
    }
    UNPROTECT(1);
    return res_;
  } else {
    error("Colour must be integer or character vector not '%s'", type2char(TYPEOF(colour_)));
  }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Bytes to hex
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
char *bytes_to_hex(uint8_t *buf) {
  static const char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  
  char *str = (char *)calloc(10, 1);
  str[0] = '#';
  str[9] = '\0';
  
  
  for (size_t i = 0; i < 4; i++) {
    str[2 * i + 1] = hexmap[(buf[i] & 0xF0) >> 4];
    str[2 * i + 2] = hexmap[ buf[i] & 0x0F];
  }
  
  return str;
}




SEXP single_int_to_col_CHARSXP(int *packed_col) {
  
  static const char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  
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
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP int_to_col_(SEXP ints_) {
  
  SEXP cols_ = PROTECT(allocVector(STRSXP, length(ints_)));
  
  int *iptr = INTEGER(ints_);
  
  for (int i = 0; i < length(ints_); i++) {
    SET_STRING_ELT(cols_, i, single_int_to_col_CHARSXP(iptr + i));
  }
  

  UNPROTECT(1);
  return cols_;
}









