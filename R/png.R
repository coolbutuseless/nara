

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Read a PNG with a transparent colour into a native raster
#' 
#' @param source Either name of the file to read from or a raw vector 
#'        representing the PNG file content.
#' @param transparent List or vector of colours which should be considered transporent
#'        R colour names, integer values, hex colours
#'                
#' @return nativeRaster
#' @import png
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
read_png <- function(source, transparent = NULL) {
  nr <- png::readPNG(source, native = TRUE)
  
  col_transparent <- colour_to_integer('#ffffff00')
  
  for (col in transparent) {
    col <- colour_to_integer(col)
    nr[nr == col] <- col_transparent
  }
  
  nr
}


if (FALSE) {
  ss <- read_png(source, transparent = list(-1594494))
  # grid.newpage(); grid.raster(ss)
  
  nr <- nr_new(100, 100, 'grey90') 
  nr_rect(nr, 1, 1, 28, 25)
  nr_blit(nr, 1, 1, ss, 7, 5, 26, 25)
  grid.newpage(); grid.raster(nr, interpolate = FALSE)
  
  nr_blit2(nr, 1, 1, ss, loc)  
  
  
}