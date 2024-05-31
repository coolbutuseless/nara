


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
  // return 1;
  return ((((colour >> 24) & 255) == 0) && (colour < 0 || colour > 31)) || colour == NA_INTEGER;
  // return ((((colour >> 24) & 255) == 0) && (colour < 0 || colour > 31)) || colour == NA_INTEGER;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int colour_to_integer(SEXP colour_) {

  const char *colour;

  if ((isLogical(colour_) && asInteger(colour_) == NA_LOGICAL)) {
    return(16777215);  //  '#ffffff00'
  } else if (isString(colour_) && asChar(colour_) == NA_STRING) {
    return(16777215);
  } else if (colour_ == NA_STRING) {
    return(16777215);
  } else if (isInteger(colour_)) {
    return(asInteger(colour_));
  } else if (isReal(colour_)) {
    return(asInteger(colour_));
  } else if (TYPEOF(colour_) == STRSXP ) {
    colour = CHAR(asChar(colour_));
  } else if (TYPEOF(colour_) == CHARSXP) {
    colour = CHAR(colour_);
  } else {
    error(">> Colour must be string or integer not %s", type2char(TYPEOF(colour_)));
  }

  if (colour[0] == '#') {
    // Rprintf("Hex colour\n");
    int n = strlen(colour);

    if (n != 7 && n != 9) {
      error("This doesn't seem like a colour: %s", colour);
    }

    unsigned long res = strtoul(colour+1,  NULL, 16);

    if (n == 7) {
      res <<= 8;
      res += 255;
    }

    return (int)reverse_bytes_32(res);
  } else {
    // Rprintf(">>>>>>>>>>>>>>> %s\n", colour);
    return rcolour_to_int(colour) ;
  }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP colour_to_integer_(SEXP colour_) {

  if (length(colour_) == 1) {
    if ((isLogical(colour_) && asInteger(colour_) == NA_LOGICAL)) {
      return(ScalarInteger(16777215));  //  '#ffffff00'
    } else if (isString(colour_) && asChar(colour_) == NA_STRING) {
      return(ScalarInteger(16777215));  //  '#ffffff00'
    }
  }

  if (isReal(colour_)) {
    SEXP res_ = PROTECT(allocVector(INTSXP, length(colour_)));

    for (int i = 0; i < length(colour_); i++) {
      INTEGER(res_)[i] = (int)REAL(colour_)[i];
    }

    UNPROTECT(1);
    return res_;
  } else if (isInteger(colour_)) {
    return(colour_);
  } else if (TYPEOF(colour_) != STRSXP) {
    error("Colours must be type '%s' or '%s' not '%s'", type2char(STRSXP), type2char(INTSXP), type2char(TYPEOF(colour_)));
  }

  SEXP res_ = PROTECT(allocVector(INTSXP, length(colour_)));

  for (int i = 0; i < length(colour_); i++) {
    INTEGER(res_)[i] = colour_to_integer(STRING_ELT(colour_, i));
  }

  UNPROTECT(1);
  return res_;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP integer_to_colour_(SEXP ints_) {
  if (!isInteger(ints_)) {
    error("Input must be integers not '%s'", type2char(TYPEOF(ints_)));
  }

  SEXP res_ = PROTECT(allocVector(STRSXP, length(ints_)));

  uint32_t *ints = (uint32_t *)INTEGER(ints_);

  for (int i=0; i<length(ints_); i++) {
    char buf[10];
    snprintf(buf, 10, "#%02x%02x%02x%02x",
            ints[i]       & 255,
            ints[i] >> 8  & 255,
            ints[i] >> 16 & 255,
            ints[i] >> 24 & 255);

    SET_STRING_ELT(res_, i, mkChar(buf));
  }

  UNPROTECT(1);
  return res_;
}








