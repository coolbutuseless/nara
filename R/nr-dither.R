
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert image to gray
#' 
#' A naive desaturation in RGB space
#' 
#' @inheritParams nr_fill
#' @param factor desaturation factor. Default: 1 (fully desaturate)
#' 
#' @return Invisibly return the supplied native raster image which was been
#'         modified in-place
#'         
#' @examples
#' nr <- nr_copy(deer_sprites[[1]])
#' plot(nr, T)
#' nr_desaturate(nr)
#' plot(nr, T)
#' @family color manipulation functions
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
#'        
#' @return Invisibly return the supplied native raster image which was been
#'         modified in-place
#'         
#' @examples
#' nr <- nr_copy(deer_sprites[[1]])
#' plot(nr, T)
#' nr_dither(nr, 0.99)
#' plot(nr, T)
#' @family color manipulation functions
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
#' 
#' @return Invisibly return the supplied native raster image which was been
#'         modified in-place
#'         
#' @examples
#' nr <- nr_copy(deer_sprites[[1]])
#' plot(nr, T)
#' nr_threshold(nr, 0.9)
#' plot(nr, T)
#' @family color manipulation functions
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_threshold <- function(nr, value = 0.5) {
  .Call(nr_threshold_, nr, value)
}

