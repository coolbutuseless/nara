


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "colour.h"
#include "nr-utils.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Assert object is a native raster
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void assert_nativeraster(SEXP nr_) {
  if (!inherits(nr_, "nativeRaster")) {
    error("Object is not a nativeRaster");
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert an R INTEGER or REAL to a C  "int *"
// Set "do_free = 1" if memory was allocated (so the parent can free() it"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int *dbl_to_int(SEXP vec_, int *do_free) {

  if (isInteger(vec_)) return INTEGER(vec_);

  if (!isReal(vec_)) {
    error("Expecting numeric but got %s\n", type2char(TYPEOF(vec_)));
  }

  *do_free = 1;
  double *vdbl = REAL(vec_);
  int *vint = malloc(length(vec_) * sizeof(int));

  for (int i=0; i<length(vec_); i++) {
    vint[i] = (int)vdbl[i];
  }

  return vint;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Extract dimensions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_dim(SEXP nr_, int *w, int *h) {
  SEXP nr_dim_ = PROTECT(GET_DIM(nr_));
  *h = INTEGER(nr_dim_)[0];
  *w = INTEGER(nr_dim_)[1];
  UNPROTECT(1);
}

