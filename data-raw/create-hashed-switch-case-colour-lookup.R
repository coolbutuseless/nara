
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Generate a big switch/case statement in C code for converting
# an R color to an integer.
# Rather than doing string compares, this is comparing hashes of the strings.
# Not really sure that an integer hash switch/case is any faster/slower
# than a strncmp() for this use case.
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cols <- colors()
zz <- purrr::map_int(cols, ~sum(utf8ToInt(.x)))

normalize_color <- function(color) {
  res <- t(col2rgb(color, alpha = TRUE))
  rgb(res[,1], res[, 2], res[,3], res[,4], maxColorValue = 255)
}

col2hex <- function(color) {
  
  color <- normalize_color(color)
  paste0(
    "0x",
    substr(color, 8, 9),
    substr(color, 6, 7),
    substr(color, 4, 5),
    substr(color, 2, 3)
  )
}


cols <- c(NA_character_, 'transparent', colors())

sink("working/col.txt")
for (color in cols) {
  cat('  case ', djb2_hash(color), ':\n    res = ', col2hex(color), '; // ', color, '\n    break;\n', sep="")
}
sink()
