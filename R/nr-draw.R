
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw points on a \code{nativeRaster} image
#'
#' @inheritParams nr_fill
#' @param x,y Vectors of point coordinates
#' @param colour Vector of colours
#'
#' @return Original \code{nativeRaster} modified in-place
#'
#' @examples
#' N <- 20
#' nr <- nr_new(N, N, 'grey80')
#' nr_point(nr, x = seq(N), y = seq(N), colour = rainbow(N)) 
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_point <- function(nr, x, y, colour = 'black') {
  invisible(.Call(draw_points_, nr, x, y, colour))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw lines on a \code{nativeRaster} image
#'
#' Uses Bresenham's algorithm to draw lines. No antialiasing.
#'
#' @inheritParams nr_fill
#' @param x0,y0,x1,y1 Vectors of coordinates of endpoints of line
#'
#' @examples
#' N <- 20
#' nr <- nr_new(N, N, 'grey80')
#' nr_line(nr, x0 = c(1, N), y0 = c(1, 1), x1 = c(N, 1), y1 = c(N, N), 
#'         colour = c('red', 'black'))
#' plot(nr)
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_line <- function(nr, x0, y0, x1, y1, colour = 'black') {
  .Call(draw_line_, nr, x0, y0, x1, y1, colour)
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
nr_text <- function(nr, x, y, str, colour = 'black', fontsize = 8L) {
  .Call(draw_text_, nr, x, y, str, colour, fontsize)
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
#' @examples
#' nr <- nr_new(400, 400)
#' nr_rect(nr, 20, 20, 100, 10)
#' 
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
  .Call(draw_polyline_, nr, x, y, colour, close)
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


if (FALSE) {
  
  N <- 20
  nr <- nr_new(400, 400)
  bench::mark(
    nr_circle(nr, 200, 200, 100, '#ff0000', colour = '#0000ff')
  )
  # 20000
    
  bench::mark(
    nr_rect(nr, 10, 10, 350, 350, '#00ff0080')
  )
  
  bench::mark(  
    nr_blit2(nr, 1, 1, deer, deer_loc[[1]])
  )
  # 280000
  
  plot(nr, T)
  
}



