

int *as_int32_vec(SEXP vec_, int N, bool *do_free);
double *as_double_vec(SEXP vec_, int N, bool *do_free);
int calc_max_length(int count, ...);
SEXP get_df_col_or_NULL (SEXP df_, const char *str);
SEXP get_df_col_or_error(SEXP df_, const char *str);

