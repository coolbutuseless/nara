
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

col2hex <- function(colour) {
  
  colour <- normalize_colour(colour)
  paste0(
    "0x",
    substr(colour, 8, 9),
    substr(colour, 6, 7),
    substr(colour, 4, 5),
    substr(colour, 2, 3)
  )
}


cols <- c(NA_character_, 'transparent', colours())

sink("working/col.txt")
for (colour in cols) {
  cat('  case ', djb2_hash(colour), ':\n    res = ', col2hex(colour), '; // ', colour, '\n    break;\n', sep="")
}
sink()