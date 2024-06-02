
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Hash a string. This hash is used for colour lookup.
#' 
#' @param str single string
#' @return integer
#' @noRd
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
djb2_hash <- function(str) {
  .Call(djb2_hash_, str)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert colours (R colours and hex colours) into packed colours 
#' (integer values containing RGBA bytes)
#'
#' @param colours character vector of R colour names and hex colours e.g. 
#'        \code{c('red', 'white', NA, 'transparent', '#12345678')}
#'
#' @return Integer vector. Each integer value contains a packed colour i.e. RGBA bytes.
#' 
#' @examples
#' str_cols_to_packed_cols(c('red', 'white', 'blue', NA, 'transparent'))
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
str_cols_to_packed_cols <- function(colours) {
  .Call(colours_to_packed_cols_, colours)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert packed colours (integer values containing RGBA bytes) to hex colours
#'
#' @param packed_cols integer values each containing packed RGBA colour information
#' 
#' @return character vector of hex colours
#'
#' @examples
#' packed_cols_to_hex_cols(c(-16776961L, -1L, -65536L, 16777215L, 16777215L))
#' 
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
packed_cols_to_hex_cols <- function(packed_cols) {
  .Call(packed_cols_to_hexcolours_, packed_cols)
}

