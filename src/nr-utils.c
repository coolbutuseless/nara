


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


int is_nativeraster(SEXP nr_) {
  return isInteger(nr_) && inherits(nr_, "nativeRaster");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Assert object is a native raster
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void assert_nativeraster(SEXP nr_) {
  if (!is_nativeraster(nr_)) {
    error("Object is not a nativeRaster");
  }
}





//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert an R INTEGER or REAL to a C  "int *"
// 
// @param vec_ R SEXP
// @param N Expected length
// @param do_Free was new memory allocat in this function (the caller should free())
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int *as_int32_vec(SEXP vec_, int N, bool *do_free) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Must be REALSXP or INTSXP
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!isInteger(vec_) && !isReal(vec_) && !isLogical(vec_)) {
    error("as_int32_vec(): Expecting numeric but got %s\n", type2char((SEXPTYPE)TYPEOF(vec_)));
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Must be length 1 or N
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (length(vec_) != 1 && length(vec_) != N) {
    error("as_int32_vec(): Input vector must be length 1 or N, not %i", length(vec_));
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Do we have an integer vector of the correct length?
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if ((isInteger(vec_) || isLogical(vec_)) && length(vec_) == N) {
    return INTEGER(vec_);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If we got here:
  //   - need to allocate memory for an integer vector
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  *do_free = 1;
  int *int_vec = malloc((size_t)N * sizeof(int));
  if (int_vec == NULL) {
    error("as_int32_vec(): out of memory");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If given INTSXP
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (isInteger(vec_) || isLogical(vec_)) {
    // Can only be an INTSXP of length=1
      int value = asInteger(vec_);
      for (int i = 0; i < N; i++) {
        int_vec[i] = value;
      }
  } else {
    // REALSXP
    double *dbl_vec = REAL(vec_);
    if (length(vec_) == N) {
      for (int i = 0; i < N; i++) {
        int_vec[i] = (int) round(dbl_vec[i]);
      }
    } else {
      // length == 1
      int value = (int)dbl_vec[0];
      for (int i = 0; i < N; i++) {
        int_vec[i] = value;
      }
    }
  }
  
  return int_vec;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert an R INTEGER or REAL to a C  "double *"
// 
// @param vec_ R SEXP
// @param N Expected length
// @param do_Free was new memory allocat in this function (the caller should free())
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double *as_double_vec(SEXP vec_, int N, bool *do_free) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Must be REALSXP or INTSXP
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!isInteger(vec_) && !isReal(vec_)) {
    error("as_double_vec(): Expecting numeric but got %s\n", type2char((SEXPTYPE)TYPEOF(vec_)));
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Must be length 1 or N
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (length(vec_) != 1 && length(vec_) != N) {
    error("as_double_vec(): Input vector must be length 1 or N, not %i", length(vec_));
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Do we have an integer vector of the correct length?
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (isReal(vec_) && length(vec_) == N) {
    return REAL(vec_);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If we got here:
  //   - need to allocate memory for an integer vector
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  *do_free = 1;
  double *dbl_vec = malloc((size_t)N * sizeof(double));
  if (dbl_vec == NULL) {
    error("as_double_vec(): out of memory");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If given INTSXP
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (isReal(vec_)) {
    // Can only be an INTSXP of length=1
    double value = asReal(vec_);
    for (int i = 0; i < N; i++) {
      dbl_vec[i] = value;
    }
  } else {
    // INTSXP
    int *int_vec = INTEGER(vec_);
    if (length(vec_) == N) {
      for (int i = 0; i < N; i++) {
        dbl_vec[i] = (double)(int_vec[i]);
      }
    } else {
      // length == 1
      double value = (double)int_vec[0];
      for (int i = 0; i < N; i++) {
        dbl_vec[i] = value;
      }
    }
  }
  
  return dbl_vec;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Calculate group N
//
// Calculate the maximum length out of a number of SEXP objects
//
// @param ... multiple numeric SEXP variables
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int calc_max_length(int count, ...) {
  va_list ap;
  va_start(ap, count);
  
  int N = -1;
  
  for (int i = 0; i < count; i++) {
    SEXP x = va_arg(ap, SEXP);
    int this_N = length(x);
    N = this_N > N ? this_N : N;
  }
  
  if (N < 0) {
    error("calc_ma_length(): -1");
  }
  
  return N;
}














