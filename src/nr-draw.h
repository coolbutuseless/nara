

void nr_point(uint32_t *nr, int height, int width, int x, int y, uint32_t color);
void nr_hline(uint32_t *nr, int height, int width, int x1, int x2, int y, uint32_t color);
void nr_line(uint32_t *nr, int height, int width, int x0, int y0, int x1, int y1, uint32_t color);
SEXP nr_polyline_(SEXP nr_, SEXP x_, SEXP y_, SEXP color_, SEXP close_);
void nr_polyline(uint32_t *nr, int height, int width, int *x, int *y, int N, uint32_t color, int close);

