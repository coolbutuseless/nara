

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Create a \code{nativeRaster} image
#'
#' A \code{nativeRaster} in R looks like an integer matrix, but is interpreted
#' differently by graphics devices:
#'
#' \itemize{
#'   \item{The data should be treated as RGBA pixels in row-major ordering}
#'   \item{Each 32-bit integer should be interpreted as 4-bytes - one for each
#'         of the R, G, B and A color channels}
#' }
#'
#' @param width,height Image dimensions in pixels
#' @param fill Background fill color as a character string. Either a standard R color
#'        (e.g. 'blue', 'white')
#'        or a hex color of the form \code{#rrggbbaa}, \code{#rrggbb}, \code{#rgba}
#'        or \code{#rgb}
#'
#' @return \code{nativeRaster}
#' 
#' @examples
#' nr <- nr_new(400, 300, 'hotpink')
#' plot(nr)
#' 
#' @importFrom colorfast col_to_int
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_new <- function(width, height, fill = 'white') {
  nr <- .Call(nr_new_, height, width)
  .Call(fill_, nr, fill)
  nr
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Fill a \code{nativeRaster} image with the given color
#'
#' @param nr \code{nativeRaster}
#' @param color Color as a character string. Either a standard R color
#'        (e.g. 'blue', 'white')
#'        or a hex color of the form \code{#rrggbbaa}, \code{#rrggbb}, \code{#rgba}
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
nr_fill <- function(nr, color) {
  invisible(.Call(fill_, nr, color))
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
#' nr2 <- nr_crop(nr, 0, 0, 10, 10)
#' dim(nr2)
#' plot(nr2)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_crop <- function(nr, x, y, w, h) {
  dst <- nr_new(w, h)
  nr_blit(dst, nr, 
          0, 0, 
          x, y, 
          w, h, respect_alpha = FALSE)
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
#' nr_rect(nr, 0, 0, 30, 15)
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
#' nr_rect(nr, 0, 0, 30, 15)
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
#' Replace colours in a native raster
#' 
#' @inheritParams nr_fill
#' @param old Vector of old colours
#' @param new Vector of replacement colours
#' 
#' @return Original \code{nativeRaster} modified in-place
#' 
#' @examples
#' nr <- nr_new(10, 10, 'hotpink')
#' nr_replace(nr, 'hotpink', 'grey80')
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_replace <- function(nr, old, new) {
  invisible(.Call(replace_, nr, old, new))
}




if (FALSE) {
  
  
  logo <- png::readPNG(system.file("img", "Rlogo.png", package="png"), native = TRUE)
  plot(logo, T)

  
  
  
    
}
