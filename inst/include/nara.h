
#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// nr_point(uint32_t *nr, int height, int width, uint32_t color, int x, int y)
// @param nr pointer to the integer data of a native raster 
// @param height,width dimensions of nativeRaster
// @param color package integer representing RGBA colour
// @param x,y location of point
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static inline void nr_point(uint32_t *nr, int height, int width, uint32_t color, int x, int y) {
  static SEXP (*fun)(uint32_t *nr, int height, int width, uint32_t color, int x, int y) = NULL;
  
  if (fun == NULL) {
    // void draw_point_c(uint32_t *nr, int height, int width, uint32_t color, int x, int y) 
    fun = (SEXP (*)(uint32_t *nr, int height, int width, uint32_t color, int x, int y)) R_GetCCallable("nara", "draw_point_c");
  }
  
  fun(nr, height, width, color, x, y);
}