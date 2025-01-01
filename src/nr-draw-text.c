
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
#include "nr-draw.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw Text
// 
// This is a simplified interface using builtin fonts. 
// Use `naratext` and/or 'lofifonts' for more options
//
// @param nr native raster (modified in-place)
// @param nr_width,nr_height dimensions
// @param x,y location
// @param str text to write
// @param color color
// @param fontsize fontsize
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "fonts.h"
void nr_text_basic(uint32_t *nr, int nr_width, int nr_height, int x, int y, const char *str, uint32_t color, int fontsize) {
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Choose font
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint8_t *font = spleen5x8;
  int char_w = 5;
  int char_h = 8;
  
  if (fontsize >= 16) {
    font = spleen8x16;
    char_w = 8;
    char_h = 16;
  } else if (fontsize >= 12) {
    font = spleen6x12;
    char_w = 6;
    char_h = 12;
  }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop over letters
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int nchars = (int)strlen(str);
  int col = 0;
  
  for (int char_idx = 0; char_idx < nchars; char_idx++) {
    int c = str[char_idx] - 32;
    if (c < 0 || c > 94) {
      Rf_error("draw_text() only accepts plain ASCII.  Char out of range: %i = %d", c, str[char_idx]);
    }
    
    uint8_t *letter = &font[c * char_h];
    for (int row = 0; row < char_h; row ++) {
      for (int i = 0; i < char_w; i++) {
        if (letter[row] & (1 << (8 - i))) {
          nr_point(nr, nr_width, nr_height, col + i + x, y - char_h + row, color);
        }
      }
    }
    
    col += char_w;
  }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw Text [R interface]
// 
// This is a simplified interface using builtin fonts. 
// Use `naratext` and/or 'lofifonts' for more options
//
// @param nr native raster (modified in-place)
// @param nr_width,nr_height dimensions
// @param x,y location
// @param str text to write
// @param color color
// @param fontsize fontsize
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_text_basic_(SEXP nr_, SEXP x_, SEXP y_, SEXP str_, SEXP color_, SEXP fontsize_) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack args
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  int  nr_height = Rf_nrows(nr_);
  int  nr_width  = Rf_ncols(nr_);

  uint32_t color = single_rcolor_to_int(color_);
  int x = Rf_asInteger(x_);
  int y = Rf_asInteger(y_);

  const char *str = CHAR(Rf_asChar(str_));

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Choose font
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int fontsize = Rf_asInteger(fontsize_);
  nr_text_basic(nr, nr_width, nr_height, x, y, str, color, fontsize);

  
  return nr_;
}


