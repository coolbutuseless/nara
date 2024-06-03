
uint32_t color_char_to_packed_col(const char *color);
uint32_t color_sexp_to_packed_col(SEXP color_);
SEXP packed_col_to_CHARSXP_color(uint32_t *packed_col);
uint32_t *colors_to_packed_cols(SEXP colors_, int N, bool *do_free);
bool is_transparent(uint32_t color);
