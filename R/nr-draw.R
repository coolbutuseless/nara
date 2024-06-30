
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw points on a \code{nativeRaster} image
#'
#' @inheritParams nr_fill
#' @param x,y Vectors of point coordinates
#' @param color Vector of colors
#'
#' @return Original \code{nativeRaster} modified in-place
#'
#' @examples
#' N <- 20
#' nr <- nr_new(N, N, 'grey80')
#' nr_point(nr, x = seq(N), y = seq(N), color = rainbow(N)) 
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_point <- function(nr, x, y, color = 'black') {
  invisible(.Call(draw_points_, nr, x, y, color))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw lines on a \code{nativeRaster} image
#'
#' Uses Bresenham's algorithm to draw lines. No antialiasing.
#'
#' @inheritParams nr_fill
#' @param x0,y0,x1,y1 Vectors of coordinates of endpoints of line
#'
#' @return Original \code{nativeRaster} modified in-place
#' 
#' @examples
#' N <- 20
#' nr <- nr_new(N, N, 'grey80')
#' nr_line(nr, x0 = c(0, N-1), y0 = c(0, 0), x1 = c(N-1, 0), y1 = c(N-1, N-1), 
#'         color = c('red', 'black'))
#' plot(nr)
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_line <- function(nr, x0, y0, x1, y1, color = 'black') {
  invisible(.Call(draw_line_, nr, x0, y0, x1, y1, color))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw text on a \code{nativeRaster} image using the built-in \code{spleen} 
#' bitmapped font.
#'
#' The only font currently available is 'spleen' - a monospace
#' bitmap font from: https://github.com/fcambus/spleen
#'
#' The 'spleen' font is licensed under BSD and the license is included in this
#' package as "LICENSE-spleen.txt". 
#' To view LICENSE: 
#' 
#' \code{cat(readLines(system.file('LICENSE-spleen.txt', package = 'nara')), 
#'  sep = "\n")}
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @param str character string
#' @param x,y coordinates of lower-left corner of text
#' @param fontsize height of font in pizels.  Only valid values are 8, 12 and 16.
#'        Default: 8.
#'
#' @return Original \code{nativeRaster} modified in-place
#'
#' @examples
#' N <- 20
#' nr <- nr_new(N, N, 'grey80')
#' nr_text_basic(nr, x = 0, y = N/2, str = "Hi!")
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_text_basic <- function(nr, x, y, str, color = 'black', fontsize = 8L) {
  invisible(.Call(draw_text_, nr, x, y, str, color, fontsize))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw rectangles on a \code{nativeRaster} image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @inheritParams nr_blit
#' @param x,y coordinates of lower left corner of rectangle.  [vector]
#' @param w,h width and height of rectangle. [vector]
#' @param color outline color. Default: NA. [vector]
#' @param fill interior fill color [vector]
#'
#' @return Original \code{nativeRaster} modified in-place
#' 
#' @examples
#' N <- 20
#' nr <- nr_new(N, N, 'grey80')
#' nr_rect(nr, x = c(0, N/2 - 1), y = c(0, N/2 - 1), w = N/2, h = N/4, 
#'         fill = 'blue', color = c('red', 'green'))
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_rect <- function(nr, x, y, w, h, fill = 'black', color = NA, hjust = 0, vjust = 0) {
  invisible(.Call(draw_rect_, nr,  x, y, w, h, fill, color, hjust, vjust))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw circles on a \code{nativeRaster} image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @param x,y coordinates of centre of circle.  [vector]
#' @param r radius [vector]
#' @param color outline color. Default: NA. [vector]
#' @param fill interior fill color [vector]
#' 
#' @return Original \code{nativeRaster} modified in-place
#'
#' @examples
#' N <- 21
#' nr <- nr_new(N, N, 'grey80')
#' nr_circle(nr, x = N/2, y = N/2, r = c(N/3, N/4), fill = c('darkred', 'black'))
#' plot(nr)
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_circle <- function(nr, x, y, r, fill = 'black', color = NA) {
  invisible(.Call(draw_circle_, nr, x, y, r, fill, color))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw polyline on a \code{nativeRaster} image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @param close Should the polyline be closed? I.e. should a line be drawn between
#'        the last point and the first point?   Default: FALSE
#'        
#' @return Original \code{nativeRaster} modified in-place
#'
#' @examples
#' N <- 20
#' nr <- nr_new(N, N, 'grey80')
#' nr_polyline(nr, x = c(0, N-1, 0), y = c(0, 0, N-1), color = 'red')
#' plot(nr)
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_polyline <- function(nr, x, y, color = 'black', close = FALSE) {
  invisible(.Call(draw_polyline_, nr, x, y, color, close))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw polygon on a \code{nativeRaster} image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @param fill fill color [scalar]
#' 
#' @return Original \code{nativeRaster} modified in-place
#'
#' @examples
#' N <- 20
#' nr <- nr_new(N, N, 'grey80')
#' nr_polygon(nr, x = c(0, N-1, 0), y = c(0, 0, N-1), fill = 'blue', color = 'red')
#' plot(nr)
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_polygon <- function(nr, x, y, fill = 'black', color = NA) {
  invisible(.Call(draw_polygon_, nr, x, y, fill, color))
}



if (FALSE) {
  
  set.seed(1)
  N <- 400
  nr <- nr_new(N, N, 'grey80')
  
  set.seed(1)
  x <- sample(400, N, T)
  y <- sample(200, N, T)
  # x <- sample(400, 10, T)
  # y <- sample( 20, 10, T)
  r <- sample(100, N, T)
  w <- sample(100, N, T)
  h <- sample(100, N, T)
  cols <- sample(colours(), N, T)
  # cols <- scales::alpha(cols, 0.5)
  
  nr_polygon(nr, x, y, fill = 'black') 
  if (FALSE) {
    nr_polygon(nr, x, y, fill = 'black') |> bench::mark()
  }
  # unoptimised: 395 
  # optimized 1000
  
  plot(nr, T)
  
}

















