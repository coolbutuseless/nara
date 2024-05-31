
// #define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Core
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP copy_into_(SEXP nr_dst_, SEXP nr_src_);
extern SEXP duplicate_(SEXP nr_);
extern SEXP fill_(SEXP nr_, SEXP colour_);
extern SEXP flipv_(SEXP nr_);
extern SEXP fliph_(SEXP nr_);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blit
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP blit_(SEXP nr_, SEXP x_, SEXP y_, SEXP src_, SEXP x0_, SEXP y0_, SEXP w_, SEXP h_);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern SEXP draw_points_  (SEXP nr_, SEXP x_ , SEXP y_                               , SEXP colour_);
extern SEXP draw_line_    (SEXP nr_, SEXP x0_, SEXP y0_, SEXP x1_, SEXP y1_          , SEXP colour_);
extern SEXP draw_text_    (SEXP nr_, SEXP x_ , SEXP y_ , SEXP str_                   , SEXP colour_, SEXP fontsize_);
extern SEXP draw_rect_    (SEXP nr_, SEXP x_ , SEXP y_ , SEXP w_, SEXP h_, SEXP fill_, SEXP colour_);
extern SEXP draw_circle_  (SEXP nr_, SEXP x_ , SEXP y_ , SEXP r_         , SEXP fill_, SEXP colour_);
extern SEXP draw_polyline_(SEXP nr_, SEXP x_ , SEXP y_                               , SEXP colour_, SEXP close_);
extern SEXP draw_polygon_ (SEXP nr_, SEXP x_ , SEXP y_                   , SEXP fill_, SEXP colour_);

extern SEXP colour_to_integer_(SEXP colour_);
extern SEXP integer_to_colour_(SEXP ints_);

extern SEXP djb2_hash_(SEXP str_);



static const R_CallMethodDef CEntries[] = {

  {"copy_into_", (DL_FUNC) &copy_into_, 2},
  {"duplicate_", (DL_FUNC) &duplicate_, 1},
  {"fill_"     , (DL_FUNC) &fill_     , 2},
  {"flipv_"    , (DL_FUNC) &flipv_    , 1},
  {"fliph_"    , (DL_FUNC) &fliph_    , 1},

  {"blit_"     , (DL_FUNC) &blit_     , 8},

  {"draw_points_"  , (DL_FUNC) &draw_points_  , 4},
  {"draw_line_"    , (DL_FUNC) &draw_line_    , 6},
  {"draw_text_"    , (DL_FUNC) &draw_text_    , 6},
  {"draw_rect_"    , (DL_FUNC) &draw_rect_    , 7},
  {"draw_circle_"  , (DL_FUNC) &draw_circle_  , 6},
  {"draw_polyline_", (DL_FUNC) &draw_polyline_, 5},
  {"draw_polygon_" , (DL_FUNC) &draw_polygon_ , 5},

  {"colour_to_integer_"  , (DL_FUNC) &colour_to_integer_  , 1},
  {"integer_to_colour_"  , (DL_FUNC) &integer_to_colour_  , 1},

  {"djb2_hash_"    , (DL_FUNC) &djb2_hash_    , 1},


  {NULL , NULL, 0}
};


void R_init_nara(DllInfo *info) {
  R_registerRoutines(
    info,      // DllInfo
    NULL,      // .C
    CEntries,  // .Call
    NULL,      // Fortran
    NULL       // External
  );
  R_useDynamicSymbols(info, FALSE);
}



