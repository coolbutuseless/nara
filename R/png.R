
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Read PNG as \code{nativeRaster} converting specified colours to transparent
#' 
#' @param source Either name of the file to read from or a raw vector 
#'        representing the PNG file content.
#' @param make_transparent List or vector of colours which should be considered transporent.
#'        R colour names, hex colours, or packed colours (integers)
#'                
#' @return \code{nativeRaster}
#' 
#' @import png
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
read_png <- function(source, make_transparent = NULL) {
  nr <- png::readPNG(source, native = TRUE)
  
  for (col in make_transparent) {
    col <- str_cols_to_packed_cols(col)
    nr[nr == col] <- transparent
  }
  
  nr
}

