

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
#' @return Integer vector. Each integer value contains RGBA bytes.
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
str_cols_to_packed_cols <- function(colours) {
  .Call(colours_to_packed_cols_, colours)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert packed colours (integer values containing RGBA bytes) to hex colours
#'
#' @param packed_cols integer values each containing packed RGBA colour information
#' @return character vector of hex colours
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
packed_cols_to_hex_cols <- function(packed_cols) {
  .Call(packed_cols_to_hexcolours_, packed_cols)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Inline benchmarks
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if (FALSE) {
  
  nr <- nr_new(500, 500)
  nr_rect(nr, x = c(10, 50), y = 20, w = 20, h = 20, fill = c('red', 'blue'))
  plot(nr, T)

  bench::mark(  
    farver::encode_native(colours()),
    str_cols_to_packed_cols(colours())
  )
  
  bench::mark(  
    farver::encode_native(colour_hex),
    str_cols_to_packed_cols(colour_hex)
  )
  
  
  res <- farver::decode_native(colour_ints)
  res <- ifelse(nchar(res) == 7, paste0(res, "FF"), res)
  identical(res, packed_cols_to_hex_cols(colour_ints))
  
  bench::mark(  
    farver::decode_native(colour_ints),
    packed_cols_to_hex_cols(colour_ints),
    check = FALSE
  )
}





