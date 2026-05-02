
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Move image colors to gray
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
  invisible(
    .Call(nr_desaturate_, nr, factor)
  )
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
  invisible(
    .Call(nr_dither_, nr, value, algo)
  )
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
  invisible(
    .Call(nr_threshold_, nr, value)
  )
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Replace colors in a native raster image
#' 
#' @inheritParams nr_fill
#' @param old Vector of old colors
#' @param new Vector of replacement colors
#' 
#' @return Invisibly return the supplied native raster image which was been
#'         modified in-place
#' 
#' @examples
#' nr <- nr_new(10, 10, 'hotpink')
#' nr_color_replace(nr, 'hotpink', 'grey80')
#' plot(nr)
#' @family color manipulation functions
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_color_replace <- function(nr, old, new) {
  invisible(
    .Call(replace_, nr, old, new)
  )
}

