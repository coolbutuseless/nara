
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Check if object is \code{nativeRaster}
#' 
#' @param x object to check
#' 
#' @return logical. TRUE if object is a \code{nativeRaster}
#' 
#' @examples
#' is_nativeraster(mtcars)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
is_nativeraster <- function(x) {
  inherits(x, 'nativeRaster') && attr(x, 'channels', exact = TRUE) %in% c(3L, 4L)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Plot a \code{nativeRaster} as an image
#' 
#' @param x \code{nativeRaster}
#' @param y any argument here will cause \code{grid::grid.newpage()} to 
#'        be called prior to drawing the \code{nativeRaster} 
#' @param ... other arguments passed to \code{grid::grid.raster()}
#'
#' @return None.
#' 
#' @examples
#' nr <- nr_new(200, 100, 'hotpink')
#' plot(nr)
#' 
#' @import grid
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
plot.nativeRaster <- function(x, y, ...) {
  if (!missing(y)) grid::grid.newpage()
  grid::grid.raster(x, interpolate = FALSE, ...)
}