
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
// @param color packed integer representing RGBA colour
// @param x,y location of point
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static inline void nr_point(uint32_t *nr, int height, int width, int x, int y, uint32_t color) {
  static SEXP (*fun)(uint32_t *nr, int height, int width, int x, int y, uint32_t color) = NULL;
  
  if (fun == NULL) {
    // void nr_point(uint32_t *nr, int height, int width, uint32_t color, int x, int y) 
    fun = (SEXP (*)(uint32_t *nr, int height, int width, int x, int y, uint32_t color)) R_GetCCallable("nara", "nr_point");
  }
  
  fun(nr, height, width, x, y, color);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// void line(uint32_t *nr, int height, int width, uint32_t color, int x0, int y0, int x1, int y1)
// @param nr pointer to the integer data of a native raster 
// @param height,width dimensions of nativeRaster
// @param color packed integer representing RGBA colour
// @param x0,y0,x1,y1 locations of endpoints
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static inline void nr_line(uint32_t *nr, int height, int width, int x0, int y0, int x1, int y1, uint32_t color) {
  static SEXP (*fun)(uint32_t *nr, int height, int width, int x0, int y0, int x1, int y1, uint32_t color) = NULL;
  
  if (fun == NULL) {
    fun = (SEXP (*)(uint32_t *nr, int height, int width, int x0, int y0, int x1, int y1, uint32_t color)) R_GetCCallable("nara", "nr_line");
  }
  
  fun(nr, height, width, x0, y0, x1, y1, color);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// void nr_hline(uint32_t *nr, int height, int width, uint32_t color, int x1, int x2, int y)
// @param nr pointer to the integer data of a native raster 
// @param height,width dimensions of nativeRaster
// @param color packed integer representing RGBA colour
// @param x1,x2 inclusive endpoints
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static inline void nr_hline(uint32_t *nr, int height, int width, int x1, int x2, int y, uint32_t color) {
  static SEXP (*fun)(uint32_t *nr, int height, int width, int x1, int x2, int y, uint32_t color) = NULL;
  
  if (fun == NULL) {
    fun = (SEXP (*)(uint32_t *nr, int height, int width, int x1, int x2, int y, uint32_t color)) R_GetCCallable("nara", "nr_hline");
  }
  
  fun(nr, height, width, x1, x2, y, color);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// void nr_circle(uint32_t *nr, int height, int width, int xm, int ym, int r, uint32_t fill, uint32_t color)
// @param nr pointer to the integer data of a native raster 
// @param height,width dimensions of nativeRaster
// @param color packed integer representing RGBA colour
// @param fill packed integer representing RGBA fill
// @param xm,ym centre
// @param r radius
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static inline void nr_circle(uint32_t *nr, int height, int width, int xm, int ym, int r, uint32_t fill, uint32_t color) {
  static SEXP (*fun)(uint32_t *nr, int height, int width, int xm, int ym, int r, uint32_t fill, uint32_t color) = NULL;
  
  if (fun == NULL) {
    fun = (SEXP (*)(uint32_t *nr, int height, int width, int xm, int ym, int r, uint32_t fill, uint32_t color)) R_GetCCallable("nara", "nr_circle");
  }
  
  fun(nr, height, width, xm, ym, r, fill, color);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// void nr_polygon(uint32_t *nr, int height, int width, uint32_t color, int *x, int *y, int npoints);
// @param nr pointer to the integer data of a native raster 
// @param height,width dimensions of nativeRaster
// @param color packed integer representing RGBA colour
// @param fill packed integer representing RGBA fill
// @param x,y integer vectors of coordinates
// @param npoints length of x and y vectors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static inline void nr_polygon(uint32_t *nr, int height, int width, int *x, int *y, int npoints, uint32_t color) {
  static SEXP (*fun)(uint32_t *nr, int height, int width, int *x, int *y, int npoints, uint32_t color) = NULL;
  
  if (fun == NULL) {
    fun = (SEXP (*)(uint32_t *nr, int height, int width, int *x, int *y, int npoints, uint32_t color)) R_GetCCallable("nara", "nr_polygon");
  }
  
  fun(nr, height, width, x, y, npoints, color);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// void nr_text_basic(uint32_t *nr, int height, int width, int x, int y, const char *str, uint32_t color, int fontsize)
// @param nr pointer to the integer data of a native raster 
// @param height,width dimensions of nativeRaster
// @param color packed integer representing RGBA colour
// @param fill packed integer representing RGBA fill
// @param x,y integer vectors of coordinates
// @param npoints length of x and y vectors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static inline void nr_text_basic(uint32_t *nr, int height, int width, int x, int y, const char *str, uint32_t color, int fontsize) {
  static SEXP (*fun)(uint32_t *nr, int height, int width, int x, int y, const char *str, uint32_t color, int fontsize) = NULL;
  
  if (fun == NULL) {
    fun = (SEXP (*)(uint32_t *nr, int height, int width, int x, int y, const char *str, uint32_t color, int fontsize)) R_GetCCallable("nara", "nr_text_basic");
  }
  
  fun(nr, height, width, x, y, str, color, fontsize);
}

