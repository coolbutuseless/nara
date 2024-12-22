
uint32_t single_rcolor_to_int(SEXP color_);
uint32_t *multi_rcolors_to_ints(SEXP colors_, int N, bool *do_free);

bool is_transparent(uint32_t color);
bool is_opaque(uint32_t color);
