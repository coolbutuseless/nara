
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
#include "nr-draw.h"
#include "nr-blit-ortho.h"
#include "nr-blit-rotozoom.h"


void nr_blit(uint32_t *dst, int dst_width, int dst_height, int x, int y, uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, int w, int h, double hjust, double vjust, double theta, double sf, bool respect_alpha) {
  
  if (theta == 0 && sf == 1) {
    nr_blit_ortho(
      dst, dst_width, dst_height, x   , y,
      src, src_width, src_height, xsrc, ysrc,
      w, h, 
      hjust, vjust,
      respect_alpha
    );
  } else {
    nr_blit_rotozoom(
      dst, dst_width, dst_height, x    , y, 
      src, src_width, src_height, xsrc , ysrc, 
      w, h,
      hjust, vjust,
      theta, sf, 
      respect_alpha
    );
  }
  
}


SEXP nr_blit_(SEXP dst_, SEXP x_, SEXP y_, 
              SEXP src_, SEXP xsrc_, SEXP ysrc_, 
              SEXP w_    , SEXP h_, 
              SEXP hjust_, SEXP vjust_, 
              SEXP angle_, SEXP sf_,
              SEXP respect_alpha_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Sanity check
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(dst_);
  assert_nativeraster(src_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack rasters
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  const int dst_width  = Rf_ncols(dst_);
  const int dst_height = Rf_nrows(dst_);
  const int src_width  = Rf_ncols(src_);
  const int src_height = Rf_nrows(src_);
  
  uint32_t *dst = (uint32_t *)INTEGER(dst_);
  uint32_t *src = (uint32_t *)INTEGER(src_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack args. Extend to vectors if required
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int N = calc_max_length(4 , x_, y_, angle_, sf_);
  
  bool freex = false, freey = false, freesf= false, freetheta = false;
  int *xs        = as_int32_vec (x_    , N, &freex);
  int *ys        = as_int32_vec (y_    , N, &freey);
  double *sfs    = as_double_vec(sf_   , N, &freesf);
  double *thetas = as_double_vec(angle_, N, &freetheta);
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Only a Single native raster allowed as input.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int xsrc = Rf_asInteger(xsrc_);
  int ysrc = Rf_asInteger(ysrc_);
  
  int w = Rf_asInteger(w_) < 0 ? src_width  : Rf_asInteger(w_);
  int h = Rf_asInteger(h_) < 0 ? src_height : Rf_asInteger(h_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // For each of the locations in the destination, blit from the source
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  for (int i = 0; i < N; i++) {
    nr_blit(dst, dst_width, dst_height, xs[i], ys[i], 
            src, src_width, src_height, xsrc , ysrc, 
            w, h,
            Rf_asReal(hjust_), Rf_asReal(vjust_),
            thetas[i], sfs[i], Rf_asLogical(respect_alpha_));
  }
  
  
  
  if (freex) free(xs);
  if (freey) free(ys);
  if (freesf) free(sfs);
  if (freetheta) free(thetas);
  
  return dst_;
  
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_blit_bulk_(SEXP dst_, SEXP src_, SEXP config_) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Check arguments are of correct type
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(dst_);
  if (!Rf_isNewList(src_)) {
    Rf_error("'src' MUST be a list of native rasters");
  }
  int Nsrc = Rf_length(src_);
  for (int i = 0; i < Nsrc; ++i) {
    assert_nativeraster(VECTOR_ELT(src_, i));
  }
  if (!Rf_inherits(config_, "data.frame")) {
    Rf_error("Config must be a data.frame");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack the config arguments and convert to correct type if necessary
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int N = Rf_length(VECTOR_ELT(config_, 0));
  if (N == 0) {
    Rf_warning("Empty 'config'");
    return dst_;
  }
  
  SEXP idx_           = get_df_col_or_error(config_, "idx"); // required
  SEXP x_             = get_df_col_or_error(config_, "x");   // required
  SEXP y_             = get_df_col_or_error(config_, "y");   // required
  
  SEXP xsrc_          = get_df_col_or_NULL(config_, "xsrc");          // def: 0
  SEXP ysrc_          = get_df_col_or_NULL(config_, "ysrc");          // def: 0
  SEXP w_             = get_df_col_or_NULL(config_, "w");             // def: src_width
  SEXP h_             = get_df_col_or_NULL(config_, "h");             // def: src_height
  SEXP hjust_         = get_df_col_or_NULL(config_, "hjust");         // def: 0
  SEXP vjust_         = get_df_col_or_NULL(config_, "vjust");         // def: 0
  SEXP respect_alpha_ = get_df_col_or_NULL(config_, "respect_alpha"); // def: TRUE
  SEXP render_        = get_df_col_or_NULL(config_, "render");        // def: TRUE
  SEXP angle_         = get_df_col_or_NULL(config_, "angle");         // def: 0.0
  SEXP scale_         = get_df_col_or_NULL(config_, "scale");         // def: 1.0
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Required
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool freeidx = false, freex = false, freey = false;
  int *idx   = as_int32_vec(idx_, N, &freeidx);
  int *x     = as_int32_vec(x_  , N, &freex);
  int *y     = as_int32_vec(y_  , N, &freey);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Optional. If not present then use default.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool freexsrc = false, freeysrc = false;
  bool freew = false, freeh = false;
  int *xsrc          = Rf_isNull(xsrc_)  ? NULL : as_int32_vec(xsrc_, N, &freexsrc);
  int *ysrc          = Rf_isNull(ysrc_)  ? NULL : as_int32_vec(ysrc_, N, &freeysrc);
  int *w             = Rf_isNull( w_)    ? NULL : as_int32_vec(w_   , N, &freew);
  int *h             = Rf_isNull( h_)    ? NULL : as_int32_vec(h_   , N, &freeh);
  double *hjust      = Rf_isNull(hjust_) ? NULL : REAL(hjust_);
  double *vjust      = Rf_isNull(vjust_) ? NULL : REAL(vjust_);
  int *respect_alpha = Rf_isNull(respect_alpha_) ? NULL : LOGICAL(respect_alpha_);
  int *render        = Rf_isNull(render_)        ? NULL : LOGICAL(render_);
  double *angle      = Rf_isNull(angle_)         ? NULL : REAL(angle_);
  double *scale      = Rf_isNull(scale_)         ? NULL : REAL(scale_);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Do the loop
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint32_t *dst = (uint32_t *)INTEGER(dst_);
  int dst_width  = Rf_ncols(dst_);
  int dst_height = Rf_nrows(dst_);
  
  for (int i = 0; i < N; ++i) {
    if (render != NULL && !render[i]) continue;
    int this_idx = idx[i] - 1;
    if (this_idx < 0 || this_idx >= Nsrc) {
      Rf_error("idx = %i at row %i is invalid. Not in range [1, %i]", idx[i], i, Nsrc);
      // continue;
    }
    SEXP this_src_ = VECTOR_ELT(src_, this_idx);
    uint32_t *src = (uint32_t *)INTEGER(this_src_);
    int src_width  = Rf_ncols(this_src_);
    int src_height = Rf_nrows(this_src_);
    
    int this_xsrc = (xsrc == NULL) ? 0 : xsrc[i];
    int this_ysrc = (ysrc == NULL) ? 0 : ysrc[i];

    int this_w = (w == NULL || w[i] < 0) ? src_width  : w[i];
    int this_h = (h == NULL || h[i] < 0) ? src_height : h[i];
    
    double this_hjust = (hjust == NULL) ? 0.5 : hjust[i];
    double this_vjust = (vjust == NULL) ? 0.5 : vjust[i];
    
    double this_angle = (angle == NULL) ? 0 : angle[i];
    double this_scale = (scale == NULL) ? 1 : scale[i];
    
    int this_res_alpha = (respect_alpha == NULL) ? 1 : respect_alpha[i];
    
    nr_blit(
      dst, dst_width, dst_height, x[i]   , y[i],
      src, src_width, src_height, this_xsrc, this_ysrc,
      this_w, this_h, 
      this_hjust, this_vjust,
      this_angle, this_scale,
      this_res_alpha);
  }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Tidy and return
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (freeidx) free(idx);
  if (freex)   free(x);
  if (freey)   free(y);
  if (freew)   free(w);
  if (freeh)   free(h);
  if (freexsrc)  free(xsrc);
  if (freeysrc)  free(ysrc);
  return dst_;
}











