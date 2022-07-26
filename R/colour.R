


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Hash a string
#' @param str single string
#' @return integer
#' @noRd
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
djb2_hash <- function(str) {
  .Call(djb2_hash_, str)
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Generate a big switch/case statement in C code for converting
# an R colour to an integer.
# Rather than doing string compares, this is comparing hashes of the strings.
# Not really sure that an integer hash switch/case is any faster/slower
# than a strncmp() for this use case.
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if (FALSE) {
  cols <- colours()
  zz <- purrr::map_int(cols, ~sum(utf8ToInt(.x)))

  normalize_colour <- function(colour) {
    res <- t(col2rgb(colour, alpha = TRUE))
    rgb(res[,1], res[, 2], res[,3], res[,4], maxColorValue = 255)
  }



  sink("col.txt")
  for (colour in colours()) {
    cat('  case ', djb2_hash(colour), ':\n    res = ', colour_to_integer(normalize_col(colour)), ';\n    break;\n', sep="")
  }
  sink()
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert a colour as a character string  into an integer value representing RGBA
#'
#' @param colour character vector of R colour names (including hex colours)
#'
#' @importFrom grDevices col2rgb
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
colour_to_integer <- function(colour) {
  .Call(colour_to_integer_, colour)
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert integer values to hex colours
#'
#' @param ints integer values
#' @return character vector of hex colours
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
integer_to_colour <- function(ints) {
  .Call(integer_to_colour_, ints)
}

