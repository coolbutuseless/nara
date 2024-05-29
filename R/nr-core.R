

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





if (FALSE) {
  N <- 400
  nr <- nr_new(N*2, N, 'grey90')
  cat("\n\n")
  nr_rect(nr, 1, 1, 5, 5, fill = 'blue')

  NC <- 300
  nr_circle(nr,
            x = sample(N*2, NC),
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

















