
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
  inherits(x, 'nativeRaster')
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
#' @importFrom grDevices dev.flush dev.hold
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
plot.nativeRaster <- function(x, y, ...) {
  if (!missing(y)) grid::grid.newpage()
  grDevices::dev.hold()
  grid::grid.raster(x, interpolate = FALSE, ...)
  grDevices::dev.flush()
  invisible(x)
}