
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
  invisible(.Call(nr_point_, nr, x, y, color))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw lines on a \code{nativeRaster} image
#'
#' Uses Bresenham's algorithm to draw lines. No antialiasing.
#'
#' @inheritParams nr_fill
#' @param x1,y1,x2,y2 Vectors of coordinates of endpoints of line
#'
#' @return Original \code{nativeRaster} modified in-place
#' 
#' @examples
#' N <- 20
#' nr <- nr_new(N, N, 'grey80')
#' nr_line(nr, x1 = c(0, N-1), y1 = c(0, 0), x2 = c(N-1, 0), y2 = c(N-1, N-1), 
#'         color = c('red', 'black'))
#' plot(nr)
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_line <- function(nr, x1, y1, x2, y2, color = 'black') {
  invisible(.Call(nr_line_, nr, x1, y1, x2, y2, color))
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
  invisible(.Call(nr_text_basic_, nr, x, y, str, color, fontsize))
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
  invisible(.Call(nr_rect_, nr,  x, y, w, h, fill, color, hjust, vjust))
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
#' N <- 201
#' nr <- nr_new(N, N, 'grey80')
#' nr_circle(nr, x = N/2, y = N/2, r = c(N/3, N/4), fill = c('darkred', 'black'))
#' plot(nr, TRUE)
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_circle <- function(nr, x, y, r, fill = 'black', color = NA) {
  invisible(.Call(nr_circle_, nr, x, y, r, fill, color))
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
#' plot(nr, T)
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_polyline <- function(nr, x, y, color = 'black', close = FALSE) {
  invisible(.Call(nr_polyline_, nr, x, y, color, close))
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw thick polyline on a \code{nativeRaster} image
#'
#' @inheritParams nr_polyline
#' @param thickness Default: 1
#' @param mitre_limit Default: 1
#'        
#' @return Original \code{nativeRaster} modified in-place
#'
#' @examples
#' N <- 20
#' nr <- nr_new(N, N, 'grey80')
#' nr_polyline_thick(nr, x = c(0, N-1, 0), y = c(0, 0, N-1), color = 'red')
#' plot(nr, T)
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_polyline_thick <- function(nr, x, y, color = 'black', thickness = 1, mitre_limit = 1, close = FALSE) {
  invisible(
    .Call(nr_polyline_thick_, nr, x, y, color, thickness, mitre_limit, close)
  )
}





#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw multiple polygon on a \code{nativeRaster} image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @param fill fill color 
#' @param id integer vector used to separate coordinates into 
#'        multiple polygons. Consecutive runs of the same \code{id}
#'        value belong to the same polygon.  If NULL (the default) then 
#'        all coordinates are assumed to be vertices of a single polygon.
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
nr_polygon <- function(nr, x, y, id = NULL, fill = 'black', color = NA) {
  invisible(.Call(nr_polygons_multi_, nr, x, y, id = id, fill, color))
}



if (FALSE) {
  
  x <- c(0, 100, 100,   0,  0,  100,   25, 75, 75, 25)
  y <- c(0,   0, 100,   0, 100, 100,   25, 25, 75, 75)
  id <- c(1, 1, 1, 2, 2, 2, 8, 8, 8, 8)
  
  nr <- nr_new(100, 100)
  
  nr_polygon(nr, x = x, y = y, id = id, fill = c('grey20', 'blue', 'hotpink'), color = c('black', 'black', 'black'))
  plot(nr)
}

















