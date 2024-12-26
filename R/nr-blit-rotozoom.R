


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Blit a source image into another native raster with rotation and scaling.
#' 
#' Only implements nearest neighbour interpolation.
#' 
#' @param nr destination 
#' @param src src
#' @param x,y location to start within dst
#' @param angle angle in radians
#' @param scale zoom factor
#' @return None. \code{nr} modified in-place and returned invisibly
#' @examples
#' nr <- nr_new(200, 200)
#' sq <- nr_new(20, 20, 'darkblue')
#' nr_blit_rotozoom(nr, src = sq, x = 100, y = 100, angle = pi/3, scale = 5)
#' plot(nr)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit_rotozoom <- function(nr, src, x, y, angle, scale) {
  invisible(
    .Call(nr_blit_rotozoom_, nr, src, x, y, angle, scale)
  )
}

