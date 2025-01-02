
#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>



static inline SEXP nr_new(int nr_width, int nr_height) {
  static SEXP (*fun)(int nr_width, int nr_height) = NULL;

  if (fun == NULL) {
    fun = (SEXP (*)(int nr_width, int nr_height)) R_GetCCallable("nara", "nr_new");
  }

  return fun(nr_width, nr_height);
}


static inline bool is_nativeraster(SEXP nr_) {
  static bool (*fun)(SEXP nr_) = NULL;

  if (fun == NULL) {
    fun = (bool (*)(SEXP nr_)) R_GetCCallable("nara", "is_nativeraster");
  }

  return fun(nr_);
}


static inline void assert_nativeraster(SEXP nr_) {
  static void (*fun)(SEXP nr_) = NULL;

  if (fun == NULL) {
    fun = (void (*)(SEXP nr_)) R_GetCCallable("nara", "assert_nativeraster");
  }

  fun(nr_);
}


static inline void nr_circle (uint32_t *nr, int nr_width, int nr_height, int x , int y , int r, uint32_t fill, uint32_t color) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int x , int y , int r, uint32_t fill, uint32_t color) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int x , int y , int r, uint32_t fill, uint32_t color)) R_GetCCallable("nara", "nr_circle");
  }

  fun(nr, nr_width, nr_height, x, y, r, fill, color);
}


static inline void nr_hline (uint32_t *nr, int nr_width, int nr_height, int x1, int x2, int y, uint32_t color) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int x1, int x2, int y, uint32_t color) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int x1, int x2, int y, uint32_t color)) R_GetCCallable("nara", "nr_hline");
  }

  fun(nr, nr_width, nr_height, x1, x2, y, color);
}


static inline void nr_line (uint32_t *nr, int nr_width, int nr_height, int x1, int y1, int x2, int y2, uint32_t color) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int x1, int y1, int x2, int y2, uint32_t color) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int x1, int y1, int x2, int y2, uint32_t color)) R_GetCCallable("nara", "nr_line");
  }

  fun(nr, nr_width, nr_height, x1, y1, x2, y2, color);
}


static inline void nr_point (uint32_t *nr, int nr_width, int nr_height, int x , int y , uint32_t color) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int x , int y , uint32_t color) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int x , int y , uint32_t color)) R_GetCCallable("nara", "nr_point");
  }

  fun(nr, nr_width, nr_height, x, y, color);
}


static inline void nr_polygon (uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color)) R_GetCCallable("nara", "nr_polygon");
  }

  fun(nr, nr_width, nr_height, x, y, npoints, color);
}


static inline void nr_polyline (uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color, int close) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color, int close) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color, int close)) R_GetCCallable("nara", "nr_polyline");
  }

  fun(nr, nr_width, nr_height, x, y, npoints, color, close);
}


static inline void nr_rect (uint32_t *nr, int nr_width, int nr_height, int x , int y , int w, int h, uint32_t fill, uint32_t color, double hjust, double vjust) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int x , int y , int w, int h, uint32_t fill, uint32_t color, double hjust, double vjust) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int x , int y , int w, int h, uint32_t fill, uint32_t color, double hjust, double vjust)) R_GetCCallable("nara", "nr_rect");
  }

  fun(nr, nr_width, nr_height, x, y, w, h, fill, color, hjust, vjust);
}


static inline void nr_text_basic(uint32_t *nr, int nr_width, int nr_height, int x , int y , const char *str, uint32_t color, int fontsize) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int x , int y , const char *str, uint32_t color, int fontsize) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int x , int y , const char *str, uint32_t color, int fontsize)) R_GetCCallable("nara", "nr_text_basic");
  }

  fun(nr, nr_width, nr_height, x, y, str, color, fontsize);
}


static inline void nr_blit(uint32_t *dst, int dst_width, int dst_height, int x , int y , uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, int w, int h, double hjust, double vjust, bool respect_alpha) {
  static void (*fun)(uint32_t *dst, int dst_width, int dst_height, int x , int y , uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, int w, int h, double hjust, double vjust, bool respect_alpha) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *dst, int dst_width, int dst_height, int x , int y , uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, int w, int h, double hjust, double vjust, bool respect_alpha)) R_GetCCallable("nara", "nr_blit");
  }

  fun(dst, dst_width, dst_height, x, y, src, src_width, src_height, xsrc, ysrc, w, h, hjust, vjust, respect_alpha);
}


static inline void nr_blit_rotozoom(uint32_t *dst, int dst_width, int dst_height, int x, int y, uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, int w, int h, double hjust, double vjust, double theta, double sf, bool respect_alpha) {
  static void (*fun)(uint32_t *dst, int dst_width, int dst_height, int x, int y, uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, int w, int h, double hjust, double vjust, double theta, double sf, bool respect_alpha) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *dst, int dst_width, int dst_height, int x, int y, uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, int w, int h, double hjust, double vjust, double theta, double sf, bool respect_alpha)) R_GetCCallable("nara", "nr_blit_rotozoom");
  }

  fun(dst, dst_width, dst_height, x, y, src, src_width, src_height, xsrc, ysrc, w, h, hjust, vjust, theta, sf, respect_alpha);
}
