
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Check if object is nativeRaster
#' 
#' @param x object to check
#' @return logical
#' @examples
#' is_nativeraster(mtcars)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
is_nativeraster <- function(x) {
  inherits(x, 'nativeRaster') && attr(x, 'channels', exact = TRUE) == 4
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Plot a native raster as an image
#' 
#' @param x native raster
#' @param y any argument here will cause the page to be called using a call
#'        to \code{grid::grid.newpage()}
#' @param ... other arguments passed to \code{grid::grid.raster()}
#'
#' @return None.
#' @import grid
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
plot.nativeRaster <- function(x, y, ...) {
  if (!missing(y)) grid::grid.newpage()
  grid::grid.raster(x, interpolate = FALSE, ...)
}