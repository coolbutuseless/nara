
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