

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert image to gray
#' 
#' A naive desaturation in RGB space
#' 
#' @inheritParams nr_fill
#' @param factor desaturation factor. Default: 1 (fully desaturate)
#' @return Original \code{nativeRaster} modified in-place
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_desaturate <- function(nr, factor = 1) {
  .Call(nr_desaturate_, nr, factor)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Dither to binary image
#' 
#' @inheritParams nr_fill
#' @param value Threshold value. Default: 0.5  (valid range [0, 1])
#' @param algo Dithering algorithm. 'fs' (floyd-steinberg) or 'atkinson'. 
#'        Default: 'fs'
#' @return Original \code{nativeRaster} modified in-place
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_dither <- function(nr, value = 0.5, algo = 'fs') {
  .Call(nr_dither_, nr, value, algo)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Threshold to binary image
#' 
#' @inheritParams nr_fill
#' @param value Threshold value. Default: 0.5  (valid range [0, 1])
#' @return Original \code{nativeRaster} modified in-place
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_threshold <- function(nr, value = 0.5) {
  .Call(nr_threshold_, nr, value)
}



