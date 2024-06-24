


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Scale the size of a \code{nativeRaster} using Nearest Neighbour resizinng
#' 
#' @inheritParams nr_fill
#' @param scale scale factor
#' 
#' @return New \code{nativeRaster}
#' 
#' @examples
#' nr1 <- nr_new(200, 100, 'white')
#' nr2 <- nr_scale(nr1, 2)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_scale <- function(nr, scale) {
  nr_resize_nn(nr, scale * ncol(nr), scale * nrow(nr))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Scale a nativeRaster using nearest-neighbour interpolation
#' 
#' @param nr native raster
#' @param width,height new dimensions
#' @return new nativeRaster
#' @examples
#' stretched <- nr_resize_nn(deer_sprites[[1]], 100, 40)
#' plot(stretched)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_resize_nn <- function(nr, width, height) {
  .Call(resize_nn_, nr, width, height)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Scale with bilinear interpolation
#' 
#' @param nr native raster
#' @param width,height new dimensions
#' @return new nativeRaster
#' @examples
#' stretched <- nr_resize_bilinear(deer_sprites[[1]], 100, 40)
#' plot(stretched)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_resize_bilinear <- function(nr, width, height) {
  .Call(resize_bilinear_, nr, width, height)
}



if (FALSE) {

  
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  #  There's an off-by-one error in nr_resize_nn
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  nr <- png::readPNG(system.file("img", "Rlogo.png", package="png"), native = TRUE)  
  dim(nr)
  
  nr_scale     (nr, 7) |> plot(T)
  nr_resize_nn      (nr, 7 * ncol(nr), 7 * nrow(nr)) |> plot(T)
  
  nr_scale     (nr, 4) |> bench::mark()
  nr_resize_nn      (nr, 4 * ncol(nr), 4 * nrow(nr)) |> bench::mark()
  
  
  nr_resize_bilinear(nr, 300, 150) |> plot(T)
  
  
  nr_resize_nn      (nr, 300, 150) |> plot()
  nr_resize_bilinear(nr, 300, 150) |> plot()
  
  
  library(magick)
  # plot(nr, T)
  im <- nr_to_magick(nr)
  im2 <- image_resize(im, geometry = geometry_size_pixels(300, 150, preserve_aspect = FALSE))
  # image_resize(im, geometry = geometry_size_pixels(300, 150, preserve_aspect = FALSE)) |> bench::mark()
  # im2  
  nr2 <- magick_to_nr(im2)  
  plot(nr2, T)  
  
}


