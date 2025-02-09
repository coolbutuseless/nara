


typedef enum {
  RESPECT_ALPHA = 1,
  IGNORE_ALPHA  = 2
} draw_mode_t;



extern SEXP nr_new(int nr_width, int nr_height);
extern bool is_nativeraster(SEXP nr_);
extern void assert_nativeraster(SEXP nr_); 
extern void nr_circle    (uint32_t *nr, int nr_width, int nr_height, int x , int y , int r, uint32_t fill, uint32_t color, draw_mode_t draw_mode);
extern void nr_hline     (uint32_t *nr, int nr_width, int nr_height, int x1, int x2, int y, uint32_t color, draw_mode_t draw_mode);
extern void nr_line      (uint32_t *nr, int nr_width, int nr_height, int x1, int y1, int x2, int y2, uint32_t color, double linewidth, draw_mode_t draw_mode);
extern void nr_point     (uint32_t *nr, int nr_width, int nr_height, int x , int y , uint32_t color, draw_mode_t draw_mode);
extern void nr_polygon   (uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t fill, uint32_t color, double linewidth, double mitre_limit, draw_mode_t draw_mode);
extern void nr_polyline  (uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color, double linewidth, double mitre_limit, bool close, draw_mode_t draw_mode);
extern void nr_rect      (uint32_t *nr, int nr_width, int nr_height, int x , int y , int w, int h, uint32_t fill, uint32_t color, double hjust, double vjust, double linewidth, draw_mode_t draw_mode);
extern void nr_text_basic(uint32_t *nr, int nr_width, int nr_height, int x , int y , const char *str, uint32_t color, int fontsize, draw_mode_t draw_mode);
extern void nr_blit(uint32_t *dst, int dst_width, int dst_height, int x, int y, uint32_t *src, int src_width, int src_height, int xsrc, int ysrc, int w, int h, double hjust, double vjust, double angle, double scale, draw_mode_t draw_mode); 
