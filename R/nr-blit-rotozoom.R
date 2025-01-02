


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Blit a source image into another native raster with rotation and scaling.
#' 
#' Only implements nearest neighbour interpolation.
#' 
#' @inheritParams nr_blit
#' @param angle angle in radians
#' @param scale zoom factor
#' @return None. \code{nr} modified in-place and returned invisibly
#' @examples
#' nr <- nr_new(300, 200, 'grey80')
#' sq <- nr_new(20, 20, 'darkblue')
#' nr_blit_rotozoom(nr, src = sq, x = 100, y = 100, angle = pi/3, scale = 5)
#' plot(nr)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit_rotozoom <- function(dst, src, x, y, angle, scale, 
                             x0 = 0L, y0 = 0L, 
                             w = -1L, h = -1L,
                             hjust = 0, vjust = 0, 
                             respect_alpha = TRUE) {
  invisible(
    .Call(nr_blit_rotozoom_, 
          dst, x, y, 
          src, x0, y0, 
          w, h,
          hjust, vjust,
          angle, scale, 
          respect_alpha)
  )
}

