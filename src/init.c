
// #define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

extern SEXP copy_into_();
extern SEXP duplicate_();
extern SEXP fill_();
extern SEXP blit_();

extern SEXP isocube_();

extern SEXP draw_points_();
extern SEXP draw_line_();
extern SEXP draw_polyline_();
extern SEXP draw_polygon_();
extern SEXP draw_text_();

extern SEXP colour_to_integer_();
extern SEXP integer_to_colour_();

extern SEXP djb2_hash_();
extern SEXP draw_rect_();
extern SEXP draw_circle_();



static const R_CallMethodDef CEntries[] = {

  {"copy_into_", (DL_FUNC) &copy_into_, 2},
  {"duplicate_", (DL_FUNC) &duplicate_, 1},
  {"fill_"     , (DL_FUNC) &fill_     , 3},

  {"blit_"     , (DL_FUNC) &blit_     , 4},
  {"isocube_"  , (DL_FUNC) &isocube_  , 7},

  {"draw_points_"  , (DL_FUNC) &draw_points_  , 5},
  {"draw_line_"    , (DL_FUNC) &draw_line_    , 7},
  {"draw_polyline_", (DL_FUNC) &draw_polyline_, 6},
  {"draw_text_"    , (DL_FUNC) &draw_text_    , 6},
  {"draw_polygon_" , (DL_FUNC) &draw_polygon_ , 6},
  {"draw_rect_"    , (DL_FUNC) &draw_rect_    , 7},
  {"draw_circle_"  , (DL_FUNC) &draw_circle_  , 6},



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



