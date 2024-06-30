


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Scale the size of a \code{nativeRaster} using Nearest Neighbour resizinng
#' 
#' @inheritParams nr_fill
#' @param scale scale factor
#' @param algo 'nn' for nearest neighbour (the default), or 'bilinear' for 
#'        bilinear interpolation.
#' 
#' @return New \code{nativeRaster}
#' @examples
#' big <- nr_scale(deer_sprites[[1]], 2)
#' plot(big)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_scale <- function(nr, scale, algo = 'nn') {
  nr_resize(nr, width = scale * ncol(nr), height = scale * nrow(nr), algo = algo)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Scale a nativeRaster 
#' 
#' @param nr native raster
#' @param algo 'nn' for nearest neighbour (the default), or 'bilinear' for 
#'        bilinear interpolation.
#' @param width,height new dimensions
#' @return New \code{nativeRaster}
#' @examples
#' stretched <- nr_resize(deer_sprites[[1]], 100, 40, algo = 'nn')
#' plot(stretched)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_resize <- function(nr, width, height, algo = 'nn') {
  if (algo == 'nn') {
    .Call(resize_nn_, nr, width, height)
  } else {
    .Call(resize_bilinear_, nr, width, height)
  }
}



if (FALSE) {

  nr <- png::readPNG(system.file("img", "Rlogo.png", package="png"), native = TRUE)  
  dim(nr)
  
  nr_scale(nr, 7) |> plot(T)
  nr_resize(nr, 7 * ncol(nr), 7 * nrow(nr)) |> plot(T)
  
  nr_scale (nr, 4) |> bench::mark()
  nr_resize(nr, 4 * ncol(nr), 4 * nrow(nr)) |> bench::mark()
  nr_resize(nr, 4 * ncol(nr), 4 * nrow(nr), algo = 'bilinear') |> bench::mark()

}


