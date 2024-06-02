
int colour_char_to_packed_col(const char *colour);
int colour_sexp_to_packed_col(SEXP colour_);
SEXP packed_col_to_CHARSXP_colour(int *packed_col);
int *colours_to_packed_cols(SEXP colours_, int N, bool *do_free);
bool is_transparent(int colour);
