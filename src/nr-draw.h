
void nr_circle    (uint32_t *nr, int nr_width, int nr_height, int x , int y , int r, uint32_t fill, uint32_t color);
void nr_hline     (uint32_t *nr, int nr_width, int nr_height, int x1, int x2, int y, uint32_t color);
void nr_line      (uint32_t *nr, int nr_width, int nr_height, int x1, int y1, int x2, int y2, uint32_t color, double linewidth);
void nr_point     (uint32_t *nr, int nr_width, int nr_height, int x , int y , uint32_t color, draw_mode_t draw_mode);
void nr_polygon   (uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t fill, uint32_t color, double linewidth, double mitre_limit);
void nr_polyline  (uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color, double linewidth, double mitre_limit, bool close);
void nr_rect      (uint32_t *nr, int nr_width, int nr_height, int x , int y , int w, int h, uint32_t fill, uint32_t color, double hjust, double vjust, double linewidth);
void nr_text_basic(uint32_t *nr, int nr_width, int nr_height, int x , int y , const char *str, uint32_t color, int fontsize, draw_mode_t draw_mode);

