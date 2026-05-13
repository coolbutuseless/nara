
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw points on a native raster image
#'
#' @inheritParams nr_fill
#' @param x,y Vectors of point coordinates
#' @param color Vector of colors
#' @param use_alpha Use alpha channel when drawing? Logical. Default: TRUE 
#'
#' @return Invisibly return the supplied native raster image which was been
#'         modified in-place
#'
#' @examples
#' w <- 200
#' h <- 150
#' nr <- nr_new(w, h, 'grey80')
#' 
#' coords <- expand.grid(x = seq(w) - 1, y = seq(h) - 1)
#' cols   <- sample(rainbow(nrow(coords)))
#' nr_point(nr, x = coords$x, y = coords$y, color = cols) 
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_point <- function(nr, x, y, color = 'black', use_alpha = TRUE) {
  invisible(.Call(nr_point_, nr, x, y, color, use_alpha))
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw lines on a native raster image
#'
#' Uses Bresenham's algorithm to draw lines. No antialiasing.
#'
#' @inheritParams nr_fill
#' @inheritParams nr_polyline
#' @param x1,y1,x2,y2 Vectors of coordinates of endpoints of line
#'
#' @return Invisibly return the supplied native raster image which was been
#'         modified in-place
#' 
#' @examples
#' w <- 200
#' h <- 150
#' nr <- nr_new(w, h, 'grey80')
#' 
#' N <- 40
#' nr_line(
#'   nr,
#'   x1 = seq(0, 2*w, length.out =  N), y1 = 0,
#'   x2 = 0, y2 = seq(0, 2 * h, length.out = N),
#'   color = rainbow(N)
#' )
#' 
#' plot(nr)
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_line <- function(nr, x1, y1, x2, y2, color = 'black', linewidth = 1, use_alpha = TRUE) {
  invisible(.Call(nr_line_, nr, x1, y1, x2, y2, color, linewidth, use_alpha))
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw text on a native raster image using the built-in monospaced
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
#' @param hjust,vjust Justification of text relative to its \code{(x,y)}
#'        location.  Default \code{(0.5, 0.5)} to centre the text at (x, y)
#' @param fontsize height of font in pizels.  Only valid values are 8, 12 and 16.
#'        Default: 8.
#'
#' @return Invisibly return the supplied native raster image which was been
#'         modified in-place
#'
#' @examples
#' w <- 150
#' h <- 30
#' nr <- nr_new(w, h, 'grey80')
#' 
#' nr_text_mono(nr, x = w/2, y = h/2, str = "Hello RStats!", fontsize = 16)
#' plot(nr)
#' 
#' nr <- nr_new(w, h, 'grey80')
#' nr_text_mono(nr, x = 0, y = 0, str = "Hello RStats!", hjust = 0, vjust = 1, fontsize = 16)
#' plot(nr)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_text_mono <- function(nr, x, y, str, color = 'black', fontsize = 8L, 
                          hjust = 0.5, vjust = 0.5, use_alpha = TRUE) {
  invisible(.Call(nr_text_mono_, nr, x, y, str, color, fontsize, hjust, vjust, use_alpha))
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw rectangles on a native raster image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @inheritParams nr_blit
#' @inheritParams nr_polyline
#' @param x,y coordinates of lower left corner of rectangle.  [vector]
#' @param w,h width and height of rectangle. [vector]
#' @param color outline color. Default: NA. [vector]
#' @param fill interior fill color [vector]
#'
#' @return Invisibly return the supplied native raster image which was been
#'         modified in-place
#' 
#' @examples
#' set.seed(1)
#' w <- 200
#' h <- 150
#' nr <- nr_new(w, h, 'grey80')
#' 
#' N <- 10
#' coords <- expand.grid(x = seq(2, w, length.out = N), 
#'                       y = seq(2, h, length.out = N))
#' 
#' cols <- sample(grDevices::heat.colors(nrow(coords)))
#' 
#' nr_rect(
#'   nr,
#'   x = coords$x,
#'   y = coords$y,
#'   w = w/N - 1,
#'   h = h/N - 1,
#'   fill = cols
#' )
#' plot(nr)
#' @family drawing functions
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_rect <- function(nr, x, y, w, h, fill = 'black', color = NA, hjust = 0, vjust = 0, linewidth = 1, use_alpha = TRUE) {
  invisible(.Call(nr_rect_, nr,  x, y, w, h, fill, color, hjust, vjust, linewidth, use_alpha))
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw circles on a native raster image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @param x,y coordinates of centre of circle.  [vector]
#' @param r radius [vector]
#' @param color outline color. Default: NA. [vector]
#' @param fill interior fill color [vector]
#' 
#' @return Invisibly return the supplied native raster image which was been
#'         modified in-place
#'
#' @examples
#' set.seed(1)
#' w <- 200
#' h <- 150
#' nr <- nr_new(w, h, 'black')
#' 
#' N <- 10
#' coords <- expand.grid(x = seq(2, w, length.out = N), 
#'                       y = seq(2, h, length.out = N))
#' 
#' cols <- sample(grDevices::terrain.colors(nrow(coords)))
#' 
#' nr_circle(
#'   nr,
#'   x = coords$x,
#'   y = coords$y,
#'   r = w/N/1.3,
#'   fill = cols
#' )
#' plot(nr)
#' @family drawing functions
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_circle <- function(nr, x, y, r, fill = 'black', color = NA, use_alpha = TRUE) {
  invisible(.Call(nr_circle_, nr, x, y, r, fill, color, use_alpha))
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw a polyline on a native raster image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @param linewidth Line linewidth. Default: 1.  If \code{linewidth = 1} then a 
#'        naive version of Bresenham is used to draw the points.  If linewidth 
#'        is greater than 1, then the line is convert to a triangle strip and
#'        rendered as polygons.
#' @param mitre_limit Limit the size of the mitre when two lines meet at an 
#'        acute angle and linewidth is greater than 1. Default: same as line 
#'        linewidth which mostly looks OK.
#' @param close Should the polyline be closed? I.e. should a line be drawn between
#'        the last point and the first point?   Default: FALSE
#'        
#' @return Invisibly return the supplied native raster image which was been
#'         modified in-place
#'
#' @examples
#' w <- 200
#' h <- 150
#' nr <- nr_new(w, h, 'grey80')
#' 
#' N  <- 20
#' theta <- seq(0, 2 * pi, length.out = N)
#' xs <- w/2 + 50 * cos(theta)
#' ys <- h/2 + 50 * sin(theta)
#' 
#' nr_polyline(nr, xs, ys)
#' nr_point(nr, xs, ys, col = 'red')
#' plot(nr)
#' @family drawing functions
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_polyline <- function(nr, x, y, color = 'black', linewidth = 1, mitre_limit = linewidth, close = FALSE, use_alpha = TRUE) {
  invisible(.Call(nr_polyline_, nr, x, y, color, linewidth, mitre_limit, close, use_alpha))
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw multiple polygons on a native raster image
#'
#' @inheritParams nr_fill
#' @inheritParams nr_point
#' @inheritParams nr_polyline
#' @param fill fill color 
#' @param id integer vector used to separate coordinates into 
#'        multiple polygons. Consecutive runs of the same \code{id}
#'        value belong to the same polygon.  If NULL (the default) then 
#'        all coordinates are assumed to be vertices of a single polygon.
#' 
#' @return Invisibly return the supplied native raster image which was been
#'         modified in-place
#'
#' @examples
#' w <- 200
#' h <- 150
#' nr <- nr_new(w, h, 'grey80')
#' 
#' N  <- 20
#' theta <- seq(0, 2 * pi, length.out = N)
#' xs <- w/2 + 50 * cos(theta)
#' ys <- h/2 + 50 * sin(theta)
#' 
#' nr_polygon(nr, xs, ys, fill = 'grey50', col = 'black')
#' nr_point(nr, xs, ys, col = 'red')
#' plot(nr)
#' @family drawing functions
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_polygon <- function(nr, x, y, id = NULL, fill = 'black', color = NA, linewidth = 1, mitre_limit = linewidth, use_alpha = TRUE) {
  invisible(.Call(nr_polygons_multi_, nr, x, y, id = id, fill, color, linewidth, mitre_limit, use_alpha))
}

