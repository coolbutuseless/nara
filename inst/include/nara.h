
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
static inline void nr_point(uint32_t *nr, int height, int width, uint32_t color, int x, int y) {
  static SEXP (*fun)(uint32_t *nr, int height, int width, uint32_t color, int x, int y) = NULL;
  
  if (fun == NULL) {
    // void draw_point_c(uint32_t *nr, int height, int width, uint32_t color, int x, int y) 
    fun = (SEXP (*)(uint32_t *nr, int height, int width, uint32_t color, int x, int y)) R_GetCCallable("nara", "nr_point");
  }
  
  fun(nr, height, width, color, x, y);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// void line(uint32_t *nr, int height, int width, uint32_t color, int x0, int y0, int x1, int y1)
// @param nr pointer to the integer data of a native raster 
// @param height,width dimensions of nativeRaster
// @param color packed integer representing RGBA colour
// @param x0,y0,x1,y1 locations of endpoints
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static inline void nr_line(uint32_t *nr, int height, int width, uint32_t color, int x0, int y0, int x1, int y1) {
  static SEXP (*fun)(uint32_t *nr, int height, int width, uint32_t color, int x0, int y0, int x1, int y1) = NULL;
  
  if (fun == NULL) {
    fun = (SEXP (*)(uint32_t *nr, int height, int width, uint32_t color, int x0, int y0, int x1, int y1)) R_GetCCallable("nara", "nr_line");
  }
  
  fun(nr, height, width, color, x0, y0, x1, y1);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// void nr_hline(uint32_t *nr, int height, int width, uint32_t color, int x1, int x2, int y)
// @param nr pointer to the integer data of a native raster 
// @param height,width dimensions of nativeRaster
// @param color packed integer representing RGBA colour
// @param x1,x2 inclusive endpoints
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static inline void nr_hline(uint32_t *nr, int height, int width, uint32_t color, int x1, int x2, int y) {
  static SEXP (*fun)(uint32_t *nr, int height, int width, uint32_t color, int x1, int x2, int y) = NULL;
  
  if (fun == NULL) {
    fun = (SEXP (*)(uint32_t *nr, int height, int width, uint32_t color, int x1, int x2, int y)) R_GetCCallable("nara", "nr_hline");
  }
  
  fun(nr, height, width, color, x1, x2, y);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// void draw_circle_c(uint32_t *nr, int height, int width, int xm, int ym, int r, uint32_t fill, uint32_t color)
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



// void fill_polygon_c_new(uint32_t *nr, int height, int width, uint32_t color, int *x, int *y, int npoints);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// void fill_polygon_c_new(uint32_t *nr, int height, int width, uint32_t color, int *x, int *y, int npoints);
// @param nr pointer to the integer data of a native raster 
// @param height,width dimensions of nativeRaster
// @param color packed integer representing RGBA colour
// @param fill packed integer representing RGBA fill
// @param x,y integer vectors of coordinates
// @param npoints length of x and y vectors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static inline void nr_polygon(uint32_t *nr, int height, int width, uint32_t color, int *x, int *y, int npoints) {
  static SEXP (*fun)(uint32_t *nr, int height, int width, uint32_t color, int *x, int *y, int npoints) = NULL;
  
  if (fun == NULL) {
    fun = (SEXP (*)(uint32_t *nr, int height, int width, uint32_t color, int *x, int *y, int npoints)) R_GetCCallable("nara", "nr_polygon");
  }
  
  fun(nr, height, width, color, x, y, npoints);
}


