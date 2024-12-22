
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
#include "nr-utils.h"


int is_nativeraster(SEXP nr_) {
  return Rf_isInteger(nr_) && Rf_inherits(nr_, "nativeRaster");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Assert object is a native raster
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void assert_nativeraster(SEXP nr_) {
  if (!is_nativeraster(nr_)) {
    Rf_error("Object is not a nativeRaster");
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Create a new nativeRaster object
//
// @param height,width dimensions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_new(int height, int width) {
  // SEXP Rf_allocMatrix(SEXPTYPE type, int nrow, int ncol);
  SEXP nr_ = PROTECT(Rf_allocMatrix(INTSXP, height, width));
  SEXP class_ = PROTECT(Rf_mkString("nativeRaster"));
  SET_CLASS(nr_, class_);
  
  UNPROTECT(2);
  return nr_;
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
  if (!Rf_isInteger(vec_) && !Rf_isReal(vec_) && !Rf_isLogical(vec_)) {
    Rf_error("as_int32_vec(): Expecting numeric but got %s\n", Rf_type2char((SEXPTYPE)TYPEOF(vec_)));
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Must be length 1 or N
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (Rf_length(vec_) != 1 && Rf_length(vec_) != N) {
    Rf_error("as_int32_vec(): Input vector must be length 1 or N, not %i", Rf_length(vec_));
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Do we have an integer vector of the correct length?
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if ((Rf_isInteger(vec_) || Rf_isLogical(vec_)) && Rf_length(vec_) == N) {
    return INTEGER(vec_);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If we got here:
  //   - need to allocate memory for an integer vector
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  *do_free = 1;
  int *int_vec = malloc((size_t)N * sizeof(int));
  if (int_vec == NULL) {
    Rf_error("as_int32_vec(): out of memory");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If given INTSXP
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (Rf_isInteger(vec_) || Rf_isLogical(vec_)) {
    // Can only be an INTSXP of length=1
      int value = Rf_asInteger(vec_);
      for (int i = 0; i < N; i++) {
        int_vec[i] = value;
      }
  } else {
    // REALSXP
    double *dbl_vec = REAL(vec_);
    if (Rf_length(vec_) == N) {
      for (int i = 0; i < N; i++) {
        int_vec[i] = isnan(dbl_vec[i]) ? NA_INTEGER : (int) round(dbl_vec[i]);
      }
    } else {
      // length == 1
      int value = isnan(dbl_vec[0]) ? NA_INTEGER : (int)round(dbl_vec[0]);
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
  if (!Rf_isInteger(vec_) && !Rf_isReal(vec_)) {
    Rf_error("as_double_vec(): Expecting numeric but got %s\n", Rf_type2char((SEXPTYPE)TYPEOF(vec_)));
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Must be length 1 or N
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (Rf_length(vec_) != 1 && Rf_length(vec_) != N) {
    Rf_error("as_double_vec(): Input vector must be length 1 or N, not %i", Rf_length(vec_));
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Do we have an integer vector of the correct length?
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (Rf_isReal(vec_) && Rf_length(vec_) == N) {
    return REAL(vec_);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If we got here:
  //   - need to allocate memory for an integer vector
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  *do_free = 1;
  double *dbl_vec = malloc((size_t)N * sizeof(double));
  if (dbl_vec == NULL) {
    Rf_error("as_double_vec(): out of memory");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If given INTSXP
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (Rf_isReal(vec_)) {
    // Can only be an INTSXP of length=1
    double value = Rf_asReal(vec_);
    for (int i = 0; i < N; i++) {
      dbl_vec[i] = value;
    }
  } else {
    // INTSXP
    int *int_vec = INTEGER(vec_);
    if (Rf_length(vec_) == N) {
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
    int this_N = Rf_length(x);
    N = this_N > N ? this_N : N;
  }
  
  if (N < 0) {
    Rf_error("calc_ma_length(): -1");
  }
  
  return N;
}














