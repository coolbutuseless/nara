
#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>






typedef enum {
  RESPECT_ALPHA = 1,
  IGNORE_ALPHA  = 2
} draw_mode_t;



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


static inline void nr_circle (uint32_t *nr, int nr_width, int nr_height, int x , int y , int r, uint32_t fill, uint32_t color, draw_mode_t draw_mode) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int x , int y , int r, uint32_t fill, uint32_t color, draw_mode_t draw_mode) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int x , int y , int r, uint32_t fill, uint32_t color, draw_mode_t draw_mode)) R_GetCCallable("nara", "nr_circle");
  }

  fun(nr, nr_width, nr_height, x, y, r, fill, color, draw_mode);
}


static inline void nr_hline (uint32_t *nr, int nr_width, int nr_height, int x1, int x2, int y, uint32_t color, draw_mode_t draw_mode) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int x1, int x2, int y, uint32_t color, draw_mode_t draw_mode) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int x1, int x2, int y, uint32_t color, draw_mode_t draw_mode)) R_GetCCallable("nara", "nr_hline");
  }

  fun(nr, nr_width, nr_height, x1, x2, y, color, draw_mode);
}


static inline void nr_line (uint32_t *nr, int nr_width, int nr_height, int x1, int y1, int x2, int y2, uint32_t color, double linewidth, draw_mode_t draw_mode) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int x1, int y1, int x2, int y2, uint32_t color, double linewidth, draw_mode_t draw_mode) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int x1, int y1, int x2, int y2, uint32_t color, double linewidth, draw_mode_t draw_mode)) R_GetCCallable("nara", "nr_line");
  }

  fun(nr, nr_width, nr_height, x1, y1, x2, y2, color, linewidth, draw_mode);
}


static inline void nr_point (uint32_t *nr, int nr_width, int nr_height, int x , int y , uint32_t color, draw_mode_t draw_mode) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int x , int y , uint32_t color, draw_mode_t draw_mode) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int x , int y , uint32_t color, draw_mode_t draw_mode)) R_GetCCallable("nara", "nr_point");
  }

  fun(nr, nr_width, nr_height, x, y, color, draw_mode);
}


static inline void nr_polygon (uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t fill, uint32_t color, double linewidth, double mitre_limit, draw_mode_t draw_mode) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t fill, uint32_t color, double linewidth, double mitre_limit, draw_mode_t draw_mode) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t fill, uint32_t color, double linewidth, double mitre_limit, draw_mode_t draw_mode)) R_GetCCallable("nara", "nr_polygon");
  }

  fun(nr, nr_width, nr_height, x, y, npoints, fill, color, linewidth, mitre_limit, draw_mode);
}


static inline void nr_polyline (uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color, double linewidth, double mitre_limit, bool close, draw_mode_t draw_mode) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color, double linewidth, double mitre_limit, bool close, draw_mode_t draw_mode) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color, double linewidth, double mitre_limit, bool close, draw_mode_t draw_mode)) R_GetCCallable("nara", "nr_polyline");
  }

  fun(nr, nr_width, nr_height, x, y, npoints, color, linewidth, mitre_limit, close, draw_mode);
}


static inline void nr_rect (uint32_t *nr, int nr_width, int nr_height, int x , int y , int w, int h, uint32_t fill, uint32_t color, double hjust, double vjust, double linewidth, draw_mode_t draw_mode) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int x , int y , int w, int h, uint32_t fill, uint32_t color, double hjust, double vjust, double linewidth, draw_mode_t draw_mode) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int x , int y , int w, int h, uint32_t fill, uint32_t color, double hjust, double vjust, double linewidth, draw_mode_t draw_mode)) R_GetCCallable("nara", "nr_rect");
  }

  fun(nr, nr_width, nr_height, x, y, w, h, fill, color, hjust, vjust, linewidth, draw_mode);
}


static inline void nr_text_basic(uint32_t *nr, int nr_width, int nr_height, int x , int y , const char *str, uint32_t color, int fontsize, draw_mode_t draw_mode) {
  static void (*fun)(uint32_t *nr, int nr_width, int nr_height, int x , int y , const char *str, uint32_t color, int fontsize, draw_mode_t draw_mode) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *nr, int nr_width, int nr_height, int x , int y , const char *str, uint32_t color, int fontsize, draw_mode_t draw_mode)) R_GetCCallable("nara", "nr_text_basic");
  }

  fun(nr, nr_width, nr_height, x, y, str, color, fontsize, draw_mode);
}


static inline void nr_blit(uint32_t *dst, int dst_width, int dst_height, int x, int y, uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, int w, int h, double hjust, double vjust, double angle, double scale, draw_mode_t draw_mode) {
  static void (*fun)(uint32_t *dst, int dst_width, int dst_height, int x, int y, uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, int w, int h, double hjust, double vjust, double angle, double scale, draw_mode_t draw_mode) = NULL;

  if (fun == NULL) {
    fun = (void (*)(uint32_t *dst, int dst_width, int dst_height, int x, int y, uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, int w, int h, double hjust, double vjust, double angle, double scale, draw_mode_t draw_mode)) R_GetCCallable("nara", "nr_blit");
  }

  fun(dst, dst_width, dst_height, x, y, src, src_width, src_height, xsrc, ysrc, w, h, hjust, vjust, angle, scale, draw_mode);
}
