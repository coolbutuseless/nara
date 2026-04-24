
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Resize a native raster by specifying the output dimensions
#' 
#' @inheritParams nr_rect
#' @param algo 'nn' for nearest neighbor (the default), or 'bilinear' for 
#'        bilinear interpolation.
#' @param width,height dimensions for output image
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



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Resize a \code{nativeRaster} using a scale factor
#' 
#' @inheritParams nr_resize
#' @param scale scale factor
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

