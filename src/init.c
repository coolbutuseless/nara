
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
extern SEXP copy_into_(SEXP nr_dst_, SEXP nr_src_);
extern SEXP duplicate_(SEXP nr_);
extern SEXP fill_(SEXP nr_, SEXP color_);
extern SEXP flipv_(SEXP nr_);
extern SEXP fliph_(SEXP nr_);
extern SEXP replace_(SEXP nr_, SEXP old_cols_, SEXP new_cols_);
extern SEXP nr_rotate_(SEXP nr_, SEXP angle_);
extern SEXP nr_transpose_(SEXP nr_);

extern SEXP resize_bilinear_(SEXP nr_, SEXP width_, SEXP height_);
extern SEXP resize_nn_      (SEXP nr_, SEXP width_, SEXP height_);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP nr_blit_(SEXP dst_  , SEXP x_    , SEXP y_, 
                     SEXP src_  , SEXP xsrc_   , SEXP ysrc_, 
                    SEXP w_    , SEXP h_, 
                    SEXP hjust_, SEXP vjust_, 
                    SEXP respect_alpha_);

extern SEXP nr_blit_bulk_(SEXP dst_, SEXP src_, SEXP config_);
extern SEXP nr_blit_rotozoom_(SEXP dst_, SEXP x_, SEXP y_, 
                              SEXP src_, SEXP xsrc_, SEXP ysrc_, 
                              SEXP w_, SEXP h_, 
                              SEXP hjust_, SEXP vjust_, 
                              SEXP angle_, SEXP sf_,
                              SEXP respect_alpha_);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Converstion
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP matrix_to_nr_(SEXP mat_, SEXP palette_, SEXP fill_, SEXP min_, SEXP max_, SEXP dst_);

extern SEXP raster_to_nr_(SEXP ras_, SEXP dst_);
extern SEXP nr_to_raster_(SEXP nr_);

extern SEXP array_to_nr_(SEXP ras_, SEXP dst_);
extern SEXP nr_to_array_(SEXP nr_);

extern SEXP magick_to_nr_(SEXP im_, SEXP dst_);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP nr_point_   (SEXP nr_, SEXP x_ , SEXP y_                               , SEXP color_);
extern SEXP nr_line_    (SEXP nr_, SEXP x1_, SEXP y1_, SEXP x2_, SEXP y2_          , SEXP color_);
extern SEXP nr_text_basic_  (SEXP nr_, SEXP x_ , SEXP y_ , SEXP str_                   , SEXP color_, SEXP fontsize_);
extern SEXP nr_rect_    (SEXP nr_, SEXP x_ , SEXP y_ , SEXP w_, SEXP h_, SEXP fill_, SEXP color_, SEXP hjust_, SEXP vjust_);
extern SEXP nr_circle_  (SEXP nr_, SEXP x_ , SEXP y_ , SEXP r_         , SEXP fill_, SEXP color_);
extern SEXP nr_polyline_(SEXP nr_, SEXP x_ , SEXP y_                               , SEXP color_, SEXP close_);
extern SEXP nr_polygons_multi_(SEXP nr_, SEXP x_ , SEXP y_, SEXP id_         , SEXP fill_, SEXP color_);


static const R_CallMethodDef CEntries[] = {
  
  {"nr_new_"   , (DL_FUNC) &nr_new_   , 2},
  {"copy_into_", (DL_FUNC) &copy_into_, 2},
  {"duplicate_", (DL_FUNC) &duplicate_, 1},
  {"fill_"     , (DL_FUNC) &fill_     , 2},
  {"flipv_"    , (DL_FUNC) &flipv_    , 1},
  {"fliph_"    , (DL_FUNC) &fliph_    , 1},
  {"replace_"  , (DL_FUNC) &replace_  , 3},
  {"nr_rotate_", (DL_FUNC) &nr_rotate_, 2},
  {"nr_transpose_", (DL_FUNC) &nr_transpose_, 1},
  
  {"resize_bilinear_", (DL_FUNC) &resize_bilinear_  , 3},
  {"resize_nn_"      , (DL_FUNC) &resize_nn_        , 3},
  
  {"nr_blit_"         , (DL_FUNC) &nr_blit_         , 11},
  {"nr_blit_bulk_"    , (DL_FUNC) &nr_blit_bulk_    ,  3},
  {"nr_blit_rotozoom_", (DL_FUNC) &nr_blit_rotozoom_, 13},
  
  {"matrix_to_nr_", (DL_FUNC) &matrix_to_nr_  , 6},
  
  {"raster_to_nr_", (DL_FUNC) &raster_to_nr_  , 2},
  {"nr_to_raster_", (DL_FUNC) &nr_to_raster_  , 1},
  
  {"array_to_nr_" , (DL_FUNC) &array_to_nr_   , 2},
  {"nr_to_array_" , (DL_FUNC) &nr_to_array_   , 1},
  
  {"magick_to_nr_" , (DL_FUNC) &magick_to_nr_ , 2},

  {"nr_point_"     , (DL_FUNC) &nr_point_     , 4},
  {"nr_line_"      , (DL_FUNC) &nr_line_      , 6},
  {"nr_text_basic_", (DL_FUNC) &nr_text_basic_, 6},
  {"nr_rect_"      , (DL_FUNC) &nr_rect_      , 9},
  {"nr_circle_"    , (DL_FUNC) &nr_circle_    , 6},
  {"nr_polyline_"  , (DL_FUNC) &nr_polyline_  , 5},
  {"nr_polygons_multi_"  , (DL_FUNC) &nr_polygons_multi_  , 6},


  {NULL , NULL, 0}
};


// C funcs for export via "LinkingTo"
extern void nr_point      (uint32_t *nr, int nr_width, int nr_height, int x, int y,                               uint32_t color);
extern void nr_line       (uint32_t *nr, int nr_width, int nr_height, int x1, int y1, int x2, int y2,             uint32_t color) ;
extern void nr_hline      (uint32_t *nr, int nr_width, int nr_height, int x1, int x2, int y,                      uint32_t color);
extern void nr_circle     (uint32_t *nr, int nr_width, int nr_height, int xm, int ym, int r      , uint32_t fill, uint32_t color);
extern void nr_polygon    (uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints,                uint32_t color);
extern void  nr_text_basic(uint32_t *nr, int nr_width, int nr_height, int x, int y, const char *str, uint32_t color, int fontsize);

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
  R_RegisterCCallable("nara", "nr_point"  , (DL_FUNC) &nr_point);
  R_RegisterCCallable("nara", "nr_line"   , (DL_FUNC) &nr_line);
  R_RegisterCCallable("nara", "nr_hline"  , (DL_FUNC) &nr_hline);
  R_RegisterCCallable("nara", "nr_circle" , (DL_FUNC) &nr_circle);
  R_RegisterCCallable("nara", "nr_polygon", (DL_FUNC) &nr_polygon);
  R_RegisterCCallable("nara", "nr_text_basic", (DL_FUNC) &nr_text_basic);
}



