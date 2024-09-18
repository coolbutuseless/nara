
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
extern SEXP copy_into_(SEXP nr_dst_, SEXP nr_src_);
extern SEXP duplicate_(SEXP nr_);
extern SEXP fill_(SEXP nr_, SEXP color_);
extern SEXP flipv_(SEXP nr_);
extern SEXP fliph_(SEXP nr_);
extern SEXP replace_(SEXP nr_, SEXP old_cols_, SEXP new_cols_);

extern SEXP resize_bilinear_(SEXP nr_, SEXP width_, SEXP height_);
extern SEXP resize_nn_      (SEXP nr_, SEXP width_, SEXP height_);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP blit_(SEXP nr_, SEXP x_, SEXP y_, SEXP src_, SEXP x0_, SEXP y0_, SEXP w_, SEXP h_, 
                  SEXP hjust_, SEXP vjust_, SEXP respect_alpha_);

extern SEXP blit_list_(SEXP nr_, SEXP x_, SEXP y_, SEXP src_list_, SEXP src_idx_, SEXP hjust_, SEXP vjust_, SEXP respect_alpha_);

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
extern SEXP nr_line_    (SEXP nr_, SEXP x0_, SEXP y0_, SEXP x1_, SEXP y1_          , SEXP color_);
extern SEXP nr_text_basic_  (SEXP nr_, SEXP x_ , SEXP y_ , SEXP str_                   , SEXP color_, SEXP fontsize_);
extern SEXP nr_rect_    (SEXP nr_, SEXP x_ , SEXP y_ , SEXP w_, SEXP h_, SEXP fill_, SEXP color_, SEXP hjust_, SEXP vjust_);
extern SEXP nr_circle_  (SEXP nr_, SEXP x_ , SEXP y_ , SEXP r_         , SEXP fill_, SEXP color_);
extern SEXP nr_polyline_(SEXP nr_, SEXP x_ , SEXP y_                               , SEXP color_, SEXP close_);
extern SEXP nr_polygons_multi_(SEXP nr_, SEXP x_ , SEXP y_, SEXP id_         , SEXP fill_, SEXP color_);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Color
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP colors_to_packed_cols_(SEXP color_);
extern SEXP packed_cols_to_hexcolors_(SEXP ints_);


static const R_CallMethodDef CEntries[] = {

  {"copy_into_", (DL_FUNC) &copy_into_, 2},
  {"duplicate_", (DL_FUNC) &duplicate_, 1},
  {"fill_"     , (DL_FUNC) &fill_     , 2},
  {"flipv_"    , (DL_FUNC) &flipv_    , 1},
  {"fliph_"    , (DL_FUNC) &fliph_    , 1},
  {"replace_"  , (DL_FUNC) &replace_  , 3},
  
  {"resize_bilinear_", (DL_FUNC) &resize_bilinear_  , 3},
  {"resize_nn_"      , (DL_FUNC) &resize_nn_        , 3},
  
  {"blit_"     , (DL_FUNC) &blit_     , 11},
  {"blit_list_", (DL_FUNC) &blit_list_,  8},
  
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
  
  {"colors_to_packed_cols_"   , (DL_FUNC) &colors_to_packed_cols_   , 1},
  {"packed_cols_to_hexcolors_", (DL_FUNC) &packed_cols_to_hexcolors_, 1},


  {NULL , NULL, 0}
};


// C funcs for export via "LinkingTo"
extern void nr_point      (uint32_t *nr, int height, int width, int x, int y,                               uint32_t color);
extern void nr_line       (uint32_t *nr, int height, int width, int x0, int y0, int x1, int y1,             uint32_t color) ;
extern void nr_hline      (uint32_t *nr, int height, int width, int x1, int x2, int y,                      uint32_t color);
extern void nr_circle     (uint32_t *nr, int height, int width, int xm, int ym, int r      , uint32_t fill, uint32_t color);
extern void nr_polygon    (uint32_t *nr, int height, int width, int *x, int *y, int npoints,                uint32_t color);
extern void  nr_text_basic(uint32_t *nr, int height, int width, int x, int y, const char *str, uint32_t color, int fontsize);

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



