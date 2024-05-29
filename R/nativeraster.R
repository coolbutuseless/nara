

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Create a \code{nativeRaster} image
#'
#' A \code{nativeRaster} in R looks like an integer matrix, but is interpreted
#' different by graphics devices:
#'
#' \itemize{
#'   \item{The data should be treated as RGBA pixels in row-major ordering}
#'   \item{Each 32-bit integer should be interpreted as 4-bytes - one for each
#'         of the R, G, B and A colour channels}
#' }
#'
#' @param width,height Image dimensions in pixels
#' @param fill Background fill color as a character string. Either a standard R color
#'        or a hex colour of the form \code{#rrggbbaa} or \code{#rrggbb}
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_new <- function(width, height, fill = 'white') {
  res <- matrix(colour_to_integer(fill), nrow = height, ncol = width)
  class(res) <- 'nativeRaster'
  attr(res, 'channels') <- 4L
  res
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Fill a \code{nativeRaster} image with the given colour
#'
#' @param nr \code{nativeRaster} image
#' @param colour Color as a character string. Either a standard R color
#'        or a hex colour of the form \code{#rrggbbaa} or \code{#rrggbb}
#' \describe{
#'   \item{\code{0}}{Draw the integer value \emph{as-is} into the nativeRaster. This mode
#'         is useful when you are using an indexed palette like many old school games
#'         (like AnotherWorld) and do not want the value interpreted as a colour and
#'         alpha blended when drawing etc}
#'   \item{\code{1}}{ This operation performs
#'        a bitwise logical-or with the current value.  This is needed for
#'        rendering in the \code{AnotherWorld} game engine.}
#'   \item{\code{2 (default)}}{Regular colour handing for standard pixel handling}
#'   }
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_fill <- function(nr, colour) {
  .Call(fill_, nr, colour)
  invisible(nr)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Copy the contents of one \code{nativeRaster} into another.
#'
#' The source and destination \code{nativeRaster} images must have the same dimensions.
#'
#' If the \code{nativeRaster} images are of different sizes, use the
#' \code{nr_blit()} function.
#'
#' @param nr_src,nr_dst Source and destination \code{nativeRaster} images
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_copy_into <- function(nr_dst, nr_src) {
  .Call(copy_into_, nr_dst, nr_src)
  invisible(nr_dst)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Create a new \code{nativeRaster} image and copy the dimensions and
#' contents from an existing image
#'
#' @inheritParams nr_fill
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_duplicate <- function(nr) {
  .Call(duplicate_, nr)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw points on a \code{nativeRaster} image
#'
#' @inheritParams nr_fill
#' @param x,y Integer vectors of point/vertex coordinates
#' @param colour Single colour or character vector of colours the same length
#'        as \code{x} and \code{y}
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_point <- function(nr, x, y, colour = 'black') {
  .Call(draw_points_, nr, colour, x, y)
  invisible(nr)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw lines on a \code{nativeRaster} image
#'
#' Uses Bresenham's algorithm to draw lines. No antialiasing.
#'
#' @inheritParams nr_fill
#' @param x0,y0,x1,y1 Coordinates of endpoints of line. [vector]
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_line <- function(nr, x0, y0, x1, y1, colour = 'black') {
  .Call(draw_line_, nr, colour, x0, y0, x1, y1)
  invisible(nr)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw polyline on a \code{nativeRaster} image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @param close Should the polyline be closed? I.e. should a line be drawn between
#'        the last point and the first point?   Default: FALSE
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_polyline <- function(nr, x, y, colour = 'black', close = FALSE) {
  .Call(draw_polyline_, nr, colour, x, y, close)
  invisible(nr)
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw polygon on a \code{nativeRaster} image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @param fill fill colour [scalar]
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_polygon <- function(nr, x, y, fill = 'black', colour = NA) {
  .Call(draw_polygon_, nr, x, y, fill, colour)
  invisible(nr)
}




#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw text on a \code{nativeRaster} image
#'
#' The only font currently available is 'spleen' - a monospace
#' bitmap font from: https://github.com/fcambus/spleen
#'
#' The 'spleen' font is licensed under BSD and the license is included in this
#' package as "LICENSE-spleen.txt"
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @param str character string [scalar]
#' @param x,y coordinates of lower-left corner of text. [scalar]
#' @param fontsize height of font in pizels.  Only valid values are 8, 12 and 16.
#'        Default: 8.
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_text <- function(nr, str, x, y, colour = 'black', fontsize = 8L) {
  .Call(draw_text_, nr, str, colour, x, y, fontsize)
  invisible(nr)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw rectangles on a \code{nativeRaster} image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @param x,y coordinates of lower left corner of rectangle.  [vector]
#' @param w,h width and height of rectangle. [vector]
#' @param colour outline colour. Default: NA. [vector]
#' @param fill interior fill colour [vector]
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_rect <- function(nr, x, y, w, h, fill = 'black', colour = NA) {
  .Call(draw_rect_, nr,  x, y, w, h, fill, colour)
  invisible(nr)
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw circles on a \code{nativeRaster} image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @param x,y coordinates of centre of circle.  [vector]
#' @param r radius [vector]
#' @param colour outline colour. Default: NA. [vector]
#' @param fill interior fill colour [vector]
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_circle <- function(nr, x, y, r, fill = 'black', colour = NA) {
  .Call(draw_circle_, nr, x, y, r, fill, colour)
  invisible(nr)
}




if (FALSE) {
  N <- 400
  nr <- nr_new(N, N, 'grey90')
  cat("\n\n")
  nr_rect(nr, 1, 1, 5, 5, fill = 'blue')

  NC <- 300
  nr_circle(nr,
            x = sample(N, NC),
            y = sample(N, NC),
            r = sample(10:20, NC, TRUE),
            fill = rainbow(NC))

  # nr_rect(nr, 3, 3, 5, 5, fill = '#ff000080')
  grid::grid.newpage()
  grid::grid.raster(nr, interpolate = FALSE)

}



if (FALSE) {

  nr <- nr_new(100, 100, fill = 0)
  # nr_fill(nr, 2L)
  nr_polygon(nr, c(20, 80, 80, 20), c(20, 20, 80, 80), fill = 2, colour = 1)
  table(nr)
}

















