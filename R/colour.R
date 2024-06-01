


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
#' @return Integer value representing RGBA colour
#' @importFrom grDevices col2rgb
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
colour_to_integer <- function(colour) {
  .Call(col_to_int_, colour)
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
  .Call(int_to_col_, ints)
}


if (FALSE) {
  
  colour_to_integer("#FFFFFF00") |> integer_to_colour()
  colour_to_integer("transparent") |> integer_to_colour()
  colour_to_integer(NA_character_) |> integer_to_colour()
  
  col2rgb(c("transparent", "#FFFFFF00", NA_character_, "white"), alpha = T)
  colour_to_integer(c("transparent", "#FFFFFF00", NA_character_, "white"))
  
}

if (FALSE) {
  
  nr <- nr_new(500, 500)
  nr_rect(nr, x = c(10, 50), y = 20, w = 20, h = 20, fill = c('red', 'blue'))
  plot(nr, T)

  bench::mark(  
    farver::encode_native(colours()),
    colour_to_integer(colours())
  )
  
  bench::mark(  
    farver::encode_native(colour_hex),
    colour_to_integer(colour_hex)
  )
  
  
  res <- farver::decode_native(colour_ints)
  res <- ifelse(nchar(res) == 7, paste0(res, "FF"), res)
  identical(res, int_to_col(colour_ints))
  
  bench::mark(  
    farver::decode_native(colour_ints),
    integer_to_colour(colour_ints),
    int_to_col(colour_ints),
    check = FALSE
  )
  
  
  
}





