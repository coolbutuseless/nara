
uint32_t colour_char_to_packed_col(const char *colour);
uint32_t colour_sexp_to_packed_col(SEXP colour_);
SEXP packed_col_to_CHARSXP_colour(uint32_t *packed_col);
uint32_t *colours_to_packed_cols(SEXP colours_, int N, bool *do_free);
bool is_transparent(uint32_t colour);
