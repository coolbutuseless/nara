


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Resize a \code{nativeRaster} using a scale factor
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
#' @family resizing functions
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_scale <- function(nr, scale, algo = 'nn') {
  nr_resize(nr, width = scale * ncol(nr), height = scale * nrow(nr), algo = algo)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Resize a native raster by specifying the output dimensions
#' 
#' @param nr native raster
#' @param algo 'nn' for nearest neighbour (the default), or 'bilinear' for 
#'        bilinear interpolation.
#' @param width,height new dimensions
#' @return New \code{nativeRaster}
#' @examples
#' stretched <- nr_resize(deer_sprites[[1]], 100, 40, algo = 'nn')
#' plot(stretched)
#' @family resizing functions
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

  nr <- fastpng::read_png(system.file("image/deer-1.png", package = "nara"), type = 'nativeraster')  
  dim(nr)
  
  nr_scale(nr, 7) |> plot(T)
  nr_resize(nr, 7 * ncol(nr), 7 * nrow(nr)) |> plot(T)
  
  nr_scale (nr, 4) |> bench::mark()
  nr_resize(nr, 4 * ncol(nr), 4 * nrow(nr)) |> bench::mark()
  nr_resize(nr, 4 * ncol(nr), 4 * nrow(nr), algo = 'bilinear') |> bench::mark()

}


