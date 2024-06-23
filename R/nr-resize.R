
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Resize with interpolation
#' 
#' @param nr native raster
#' @param width,height target size
#' 
#' @return new nativeraster
#' @examples
#' nr <- nr_new(100, 100)
#' nr2 <- nr_resize(nr, 200, 10)
#' plot(nr2, TRUE)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_resize <- function(nr, width, height) {
  .Call(resize_, nr, width, height)
}



if (FALSE) {
  
  logo <- png::readPNG(system.file("img", "Rlogo.png", package="png"), native = TRUE)
  plot(logo, T)
  l2 <- nr_resize(logo, 200, 50)
  plot(l2, T)
}