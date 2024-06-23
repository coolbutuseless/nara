
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Resize with interpolation
#' 
#' @param nr native raster
#' @param width,height target size
#' 
#' @return new nativeraster
#' @examples
#' stretched_deer <- nr_resize(deer_sprites[[1]], 200, 32)
#' plot(stretched_deer, TRUE)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_resize <- function(nr, width, height) {
  .Call(resize_, nr, width, height)
}



if (FALSE) {
  
  logo <- png::readPNG(system.file("img", "Rlogo.png", package="png"), native = TRUE)
  # plot(logo, T)
  nr_resize(logo, 1600, 500) |> bench::mark()
  l2 <- nr_resize(logo, 1600, 500) #|> bench::mark()
  plot(l2, T)
  
  library(magick)
  im <- nr_to_magick(logo)
  image_resize(im, geometry = geometry_size_pixels(200, 50, preserve_aspect = FALSE))  |> bench::mark()
  
}