

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Create a \code{nativeRaster} image
#'
#' A \code{nativeRaster} in R looks like an integer matrix, but is interpreted
#' differently by graphics devices:
#'
#' \itemize{
#'   \item{The data should be treated as RGBA pixels in row-major ordering}
#'   \item{Each 32-bit integer should be interpreted as 4-bytes - one for each
#'         of the R, G, B and A colour channels}
#' }
#'
#' @param width,height Image dimensions in pixels
#' @param fill Background fill color as a character string. Either a standard R colour
#'        (e.g. 'blue', 'white')
#'        or a hex colour of the form \code{#rrggbbaa}, \code{#rrggbb}, \code{#rgba}
#'        or \code{#rgb}
#'
#' @return \code{nativeRaster}
#' 
#' @examples
#' nr <- nr_new(400, 300, 'hotpink')
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_new <- function(width, height, fill = 'white') {
  res <- matrix(str_cols_to_packed_cols(fill), nrow = height, ncol = width)
  class(res) <- 'nativeRaster'
  attr(res, 'channels') <- 4L
  res
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Fill a \code{nativeRaster} image with the given colour
#'
#' @param nr \code{nativeRaster}
#' @param colour Color as a character string. Either a standard R colour
#'        (e.g. 'blue', 'white')
#'        or a hex colour of the form \code{#rrggbbaa}, \code{#rrggbb}, \code{#rgba}
#'        or \code{#rgb}
#'        
#' @return The original \code{nativeRaster} modified in-place.        
#' 
#' @examples
#' nr <- nr_new(400, 300, 'hotpink')
#' nr_fill(nr, 'blue')
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_fill <- function(nr, colour) {
  invisible(.Call(fill_, nr, colour))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Copy the contents of one \code{nativeRaster} into another.
#'
#' The source and destination \code{nativeRaster} images must have the same dimensions.
#'
#' If the \code{nativeRaster} images are of different sizes, use the
#' \code{nr_blit()} function.
#'
#' @param src,dst Source and destination \code{nativeRaster} images
#'
#' @return The 'dst' \code{nativeRaster}
#' 
#' @examples
#' nr1 <- nr_new(200, 100, 'hotpink')
#' nr2 <- nr_new(200, 100, 'green')
#' nr_copy_into(nr1, nr2)
#' plot(nr1)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_copy_into <- function(dst, src) {
  invisible(.Call(copy_into_, dst, src))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Create a new \code{nativeRaster} image and copy the dimensions and
#' contents from an existing image
#'
#' @inheritParams nr_fill
#' 
#' @return New \code{nativeRaster}
#' 
#' @examples
#' nr1 <- nr_new(200, 200, 'hotpink')
#' nr2 <- nr_duplicate(nr1)
#' plot(nr2)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_duplicate <- function(nr) {
  .Call(duplicate_, nr)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Crop a section out of a nativeRaster into a new nativeRaster
#' 
#' @inheritParams nr_fill
#' @param x,y,w,h dimensions of cropped section
#' @param loc dimensions of cropped section. A vector of 4 values 
#'        i.e. \code{c(x, y, w, h)}
#' 
#' @return New \code{nativeRaster}
#' 
#' @examples
#' nr <- nr_new(400, 400, 'hotpink')
#' nr2 <- nr_crop(nr, 1, 1, 10, 10)
#' plot(nr2)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_crop <- function(nr, x, y, w, h) {
  dst <- nr_new(w, h)
  nr_blit(dst, 1, 1, nr, x, y, w, h)
  dst
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_crop
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_crop2 <- function(nr, loc) {
  nr_crop(nr, loc[[1]], loc[[2]], loc[[3]], loc[[4]])
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Flip a \code{nativeRaster} vertically
#' 
#' @inheritParams nr_fill
#' 
#' @return Original \code{nativeRaster} modified in-place
#' 
#' @examples
#' nr <- nr_new(400, 200, 'white')
#' nr_rect(nr, 1, 1, 30, 15)
#' plot(nr)
#' nr_flipv(nr)
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_flipv <- function(nr) {
  invisible(.Call(flipv_, nr))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Flip a \code{nativeRaster} horizontally
#' 
#' @inheritParams nr_fill
#' 
#' @return Original \code{nativeRaster} modified in-place
#' 
#' @examples
#' nr <- nr_new(400, 200, 'white')
#' nr_rect(nr, 1, 1, 30, 15)
#' plot(nr)
#' nr_fliph(nr)
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_fliph <- function(nr) {
  invisible(.Call(fliph_, nr))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Scale up a \code{nativeRaster} by an integer factor
#' 
#' @inheritParams nr_fill
#' @param scale integer value >= 1
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
  scale <- as.integer(scale)
  if (length(scale) != 1L || scale < 1L) {
    stop("Invalid 'scale' value: ", scale)
  }
  
  m <- matrix(1L, scale, scale)
  res <- base::kronecker(nr, m)
  mode(res) <- 'integer'
  
  class(res) <- 'nativeRaster'
  attr(res, 'channels') <- 4L
  res
}


