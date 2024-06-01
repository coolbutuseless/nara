
int single_str_col_to_int(const char *colour);
int single_sexp_col_to_int(SEXP colour_);
SEXP single_int_to_col_CHARSXP(int *packed_col);
int *col_to_int_ptr(SEXP colour_, int N, int *do_free);
int is_transparent(int colour);
