
// #define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Core
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP nr_new_(SEXP height_, SEXP width_);
extern SEXP nr_copy_into_(SEXP dst_, SEXP src_);
extern SEXP nr_copy_(SEXP nr_);
extern SEXP nr_fill_(SEXP nr_, SEXP color_);
extern SEXP nr_flipv_(SEXP nr_);
extern SEXP nr_fliph_(SEXP nr_);
extern SEXP nr_color_replace_(SEXP nr_, SEXP old_cols_, SEXP new_cols_);
extern SEXP nr_rotate_(SEXP nr_, SEXP angle_);
extern SEXP nr_transpose_(SEXP nr_);

extern SEXP nr_resize_bilinear_(SEXP nr_, SEXP width_, SEXP height_);
extern SEXP nr_resize_nn_      (SEXP nr_, SEXP width_, SEXP height_);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP nr_blit_ortho_(SEXP dst_  , SEXP x_    , SEXP y_,
                     SEXP src_  , SEXP xsrc_   , SEXP ysrc_,
                    SEXP w_    , SEXP h_,
                    SEXP hjust_, SEXP vjust_,
                    SEXP draw_mode_);

extern SEXP nr_blit_rotozoom_(SEXP dst_, SEXP x_, SEXP y_,
                              SEXP src_, SEXP xsrc_, SEXP ysrc_,
                              SEXP w_, SEXP h_,
                              SEXP hjust_, SEXP vjust_,
                              SEXP angle_, SEXP sf_,
                              SEXP draw_mode_);

extern SEXP nr_blit_(SEXP dst_, SEXP x_, SEXP y_, 
                     SEXP src_, SEXP xsrc_, SEXP ysrc_, 
                     SEXP w_, SEXP h_, 
                     SEXP hjust_, SEXP vjust_, 
                     SEXP angle_, SEXP sf_,
                     SEXP draw_mode_);

extern SEXP nr_blit_bulk_(SEXP dst_, SEXP src_, SEXP config_);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Conversion
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP matrix_to_nr_(SEXP mat_, SEXP palette_, SEXP nr_fill_, SEXP min_, SEXP max_, SEXP dst_);

extern SEXP raster_to_nr_(SEXP ras_, SEXP dst_);
extern SEXP nr_to_raster_(SEXP nr_);

extern SEXP array_to_nr_(SEXP ras_, SEXP dst_);
extern SEXP nr_to_array_(SEXP nr_);

extern SEXP magick_to_nr_(SEXP im_, SEXP dst_);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP nr_point_   (SEXP nr_, SEXP x_ , SEXP y_                               , SEXP color_, SEXP draw_mode_);
extern SEXP nr_line_    (SEXP nr_, SEXP x1_, SEXP y1_, SEXP x2_, SEXP y2_          , SEXP color_, SEXP linewidth_, SEXP draw_mode_);
extern SEXP nr_text_basic_  (SEXP nr_, SEXP x_ , SEXP y_ , SEXP str_                   , SEXP color_, SEXP fontsize_, SEXP draw_mode_);
extern SEXP nr_rect_    (SEXP nr_, SEXP x_ , SEXP y_ , SEXP w_, SEXP h_, SEXP nr_fill_, SEXP color_, SEXP hjust_, SEXP vjust_, SEXP linewidth_, SEXP draw_mode_);
extern SEXP nr_circle_  (SEXP nr_, SEXP x_ , SEXP y_ , SEXP r_         , SEXP nr_fill_, SEXP color_, SEXP draw_mode_);
extern SEXP nr_polyline_(SEXP nr_, SEXP x_ , SEXP y_, SEXP color_, SEXP linewidth_, SEXP mitre_limit_, SEXP close_, SEXP draw_line_);
extern SEXP nr_polygons_multi_(SEXP nr_, SEXP x_ , SEXP y_, SEXP id_         , SEXP nr_fill_, SEXP color_, 
                               SEXP linewidth_, SEXP mitre_limit_, SEXP draw_mode_);


extern SEXP nr_tri_coords_(SEXP nr_, SEXP coords_, SEXP col_, SEXP tri_mode_, 
                           SEXP draw_mode_);
extern SEXP nr_tri_mesh_(SEXP nr_, SEXP vertices_, SEXP indices_, SEXP color_, 
                         SEXP tri_mode_, SEXP draw_mode_);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Color twiddling
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP nr_desaturate_(SEXP nr_);
extern SEXP nr_dither_(SEXP nr_, SEXP value_, SEXP algo_);
extern SEXP nr_threshold_(SEXP nr_, SEXP value_);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Mask
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP nr_mask_begin_(SEXP nr_, SEXP mask_);
extern SEXP nr_mask_end_  (SEXP nr_);


static const R_CallMethodDef CEntries[] = {
  
  {"nr_new_"   , (DL_FUNC) &nr_new_   , 2},
  {"nr_copy_into_", (DL_FUNC) &nr_copy_into_, 2},
  {"nr_copy_", (DL_FUNC) &nr_copy_, 1},
  {"nr_fill_"     , (DL_FUNC) &nr_fill_     , 2},
  {"nr_flipv_"    , (DL_FUNC) &nr_flipv_    , 1},
  {"nr_fliph_"    , (DL_FUNC) &nr_fliph_    , 1},
  {"nr_color_replace_"  , (DL_FUNC) &nr_color_replace_  , 3},
  {"nr_rotate_", (DL_FUNC) &nr_rotate_, 2},
  {"nr_transpose_", (DL_FUNC) &nr_transpose_, 1},
  
  {"nr_resize_bilinear_", (DL_FUNC) &nr_resize_bilinear_  , 3},
  {"nr_resize_nn_"      , (DL_FUNC) &nr_resize_nn_        , 3},
  
  {"nr_blit_ortho_"   , (DL_FUNC) &nr_blit_ortho_   , 11}, // subsumed into 'nr_blit_'
  {"nr_blit_rotozoom_", (DL_FUNC) &nr_blit_rotozoom_, 13}, // subsumed into 'nr_blit_'
  {"nr_blit_"         , (DL_FUNC) &nr_blit_         , 13},
  {"nr_blit_bulk_"    , (DL_FUNC) &nr_blit_bulk_    ,  3},
  
  {"matrix_to_nr_", (DL_FUNC) &matrix_to_nr_  , 6},
  
  {"raster_to_nr_", (DL_FUNC) &raster_to_nr_  , 2},
  {"nr_to_raster_", (DL_FUNC) &nr_to_raster_  , 1},
  
  {"array_to_nr_" , (DL_FUNC) &array_to_nr_   , 2},
  {"nr_to_array_" , (DL_FUNC) &nr_to_array_   , 1},
  
  {"magick_to_nr_" , (DL_FUNC) &magick_to_nr_ , 2},

  {"nr_point_"         , (DL_FUNC) &nr_point_         , 5},
  {"nr_line_"          , (DL_FUNC) &nr_line_          , 8},
  {"nr_text_basic_"    , (DL_FUNC) &nr_text_basic_    , 7},
  {"nr_rect_"          , (DL_FUNC) &nr_rect_          , 11},
  {"nr_circle_"        , (DL_FUNC) &nr_circle_        , 7},
  {"nr_polyline_"      , (DL_FUNC) &nr_polyline_      , 8},
  {"nr_polygons_multi_", (DL_FUNC) &nr_polygons_multi_, 9},
  {"nr_tri_coords_"    , (DL_FUNC) &nr_tri_coords_    , 5},
  {"nr_tri_mesh_"      , (DL_FUNC) &nr_tri_mesh_      , 6},

  
  {"nr_desaturate_", (DL_FUNC) &nr_desaturate_, 2},
  {"nr_threshold_" , (DL_FUNC) &nr_threshold_,  2},
  {"nr_dither_"    , (DL_FUNC) &nr_dither_    , 3},
  
  
  {"nr_mask_begin_", (DL_FUNC) &nr_mask_begin_, 2},
  {"nr_mask_end_"  , (DL_FUNC) &nr_mask_end_  , 1},

  {NULL , NULL, 0}
};


// C funcs for export via "LinkingTo"
#include "init-decls.h" // Generated by data-raw/create-c-api-header.R

void R_init_nara(DllInfo *info) {
  R_registerRoutines(
    info,      // DllInfo
    NULL,      // .C
    CEntries,  // .Call
    NULL,      // Fortran
    NULL       // External
  );
  R_useDynamicSymbols(info, FALSE);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Make the C code available to other packages
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "init-register.h" // Generated by data-raw/create-c-api-header.R
  // R_RegisterCCallable("nara", "nr_point"  , (DL_FUNC) &nr_point);
}



