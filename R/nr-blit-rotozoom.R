


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
#'
#' nr <- nr_new(300, 200, 'grey80')
#' sq <- png::readPNG(system.file("img", "Rlogo.png", package="png"), native = TRUE)
#' nr_blit_rotozoom(nr, src = sq, x = 180, y = 120, angle = pi/6, scale = 1)
#' plot(nr)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit_rotozoom <- function(dst, src, x, y, angle, scale, 
                             xsrc = 0L, ysrc = 0L, 
                             w = -1L, h = -1L,
                             hjust = 0.5, vjust = 0.5, 
                             respect_alpha = TRUE) {
  invisible(
    .Call(nr_blit_rotozoom_, 
          dst, x, y, 
          src, xsrc, ysrc, 
          w, h,
          hjust, vjust,
          angle, scale, 
          respect_alpha)
  )
}


if (FALSE) {
  
  sq <- png::readPNG(system.file("img", "Rlogo.png", package="png"), native = TRUE)

  theta <- pi/4
  # for (theta in seq(0, 2*pi, length.out = 20)) {
  nr <- nr_new(300, 200, 'grey80')
  nr_blit_rotozoom(nr, src = sq, x = 50, y = 50,
                   hjust = 0.5, vjust = 0.5,
                   angle = 0, scale = 1)
  nr_circle(nr, 50, 50, 2, fill = 'red')
  plot(nr)
  # Sys.sleep(0.15)
  # }

}







