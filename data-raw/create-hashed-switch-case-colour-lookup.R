
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Generate a big switch/case statement in C code for converting
# an R colour to an integer.
# Rather than doing string compares, this is comparing hashes of the strings.
# Not really sure that an integer hash switch/case is any faster/slower
# than a strncmp() for this use case.
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cols <- colours()
zz <- purrr::map_int(cols, ~sum(utf8ToInt(.x)))

normalize_colour <- function(colour) {
  res <- t(col2rgb(colour, alpha = TRUE))
  rgb(res[,1], res[, 2], res[,3], res[,4], maxColorValue = 255)
}



sink("col.txt")
for (colour in colours()) {
  cat('  case ', djb2_hash(colour), ':\n    res = ', str_cols_to_packed_cols(normalize_colour(colour)), ';\n    break;\n', sep="")
}
sink()