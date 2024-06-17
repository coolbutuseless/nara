

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert colors (R colors and hex colors) into packed colors 
#' (integer values containing RGBA bytes)
#'
#' @param colors character vector of R color names and hex colors e.g. 
#'        \code{c('red', 'white', NA, 'transparent', '#12345678')}
#'
#' @return Integer vector. Each integer value contains a packed color i.e. RGBA bytes.
#' 
#' @examples
#' str_cols_to_packed_cols(c('red', 'white', 'blue', NA, 'transparent'))
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
str_cols_to_packed_cols <- function(colors) {
  .Call(colors_to_packed_cols_, colors)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert packed colors (integer values containing RGBA bytes) to hex colors
#'
#' @param packed_cols integer values each containing packed RGBA color information
#' 
#' @return character vector of hex colors
#'
#' @examples
#' packed_cols_to_hex_cols(c(-16776961L, -1L, -65536L, 16777215L, 16777215L))
#' 
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
packed_cols_to_hex_cols <- function(packed_cols) {
  .Call(packed_cols_to_hexcolors_, packed_cols)
}

