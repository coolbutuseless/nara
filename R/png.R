
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Read PNG as \code{nativeRaster} converting specified colors to transparent
#' 
#' @param source Either name of the file to read from or a raw vector 
#'        representing the PNG file content.
#' @param make_transparent List or vector of colors which should be considered transporent.
#'        R color names, hex colors, or packed colors (integers)
#'                
#' @return \code{nativeRaster}
#' 
#' @examples
#' source <- system.file("img/Rlogo.png", package = "png", mustWork = TRUE)
#' nr <- read_png(source, make_transparent = 'white')
#' plot(nr)
#' 
#' @import png
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
read_png <- function(source, make_transparent = NULL) {
  nr <- png::readPNG(source, native = TRUE)
  
  for (col in make_transparent) {
    col <- str_cols_to_packed_cols(col)
    nr[nr == col] <- transparent_white
  }
  
  nr
}

