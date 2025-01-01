
void nr_circle    (uint32_t *nr, int nr_width, int nr_height, int xm, int ym, int r, uint32_t fill, uint32_t color);
void nr_hline     (uint32_t *nr, int nr_width, int nr_height, int x1, int x2, int y, uint32_t color);
void nr_line      (uint32_t *nr, int nr_width, int nr_height, int x0, int y0, int x1, int y1, uint32_t color);
void nr_point     (uint32_t *nr, int nr_width, int nr_height, int x, int y, uint32_t color);
void nr_polygon   (uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color);
void nr_polyline  (uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int N, uint32_t color, int close);
void nr_rect      (uint32_t *nr, int nr_width, int nr_height, int x, int y, int w, int h, uint32_t fill, uint32_t color, double hjust, double vjust);
void nr_text_basic(uint32_t *nr, int nr_width, int nr_height, int x, int y, const char *str, uint32_t color, int fontsize);

