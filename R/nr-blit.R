

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Copy one \code{nativeRaster} image into another at an arbitrary location.
#'
#' This is useful as a way of positioning sprites or icons in an image.
#'
#' @param nr native raster to copy into
#' @param src native raster to copy from
#' @param x,y Where in \code{nr} to place the \code{sprite}. These values must
#'        be vectors of the same length.  If the length is greater than 1, then
#'        the \code{sprite} will be pasted into \code{nr} at multiple locations.
#'        Note that the
#'        origin of \code{nativeraster} images is the top-left
#'        where the coordinates are (0, 0).
#' @param x0,y0 start coordiates within src
#' @param w,h size within src. If size is negative, then the actual width/height of
#'        the src is used
#' @param respect_alpha Should the alpha channel be respected when blitting?
#'        Default: TRUE.  If FALSE, then contents will be blindly overwritten
#'        which can be much much faster.  If the \code{src} has an 
#'        any transparent pixels, \code{respect_alpha = TRUE} is 
#'        probably the correct setting.
#' @param hjust,vjust specify horizontal and vertical justification of the 
#'        \code{src} image.  e.g. \code{hjust = vjust = 0} the blitting
#'        starts at the top-left of the image. Use \code{hjust = vjust = 0.5}
#'        to treat the centre of the \code{src_} as the blitting origin.
#'        Default (0, 0)
#'
#' @return \code{nativeRaster}
#' 
#' @examples
#' nr <- nr_new(50, 50, 'grey80')
#' nr_blit(nr, x = 0, y = 0, src = deer_sprites[[1]])
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit <- function(nr, x, y, src, x0 = 0L, y0 = 0L, w = -1L, h = -1L, hjust = 0, vjust = 0, respect_alpha = TRUE) {
  invisible(.Call(blit_, nr, x, y, src, x0, y0, w, h, hjust, vjust, respect_alpha))
}


if (FALSE) {
  
  logo <- fastpng::read_png(system.file("img", "Rlogo.png", package="png"), type = 'nativeraster')
  
  nr <- nr_new(100, 100, 'grey80')
  idx_mat <- matrix(c(
    1, 2, 3,
    4, 5, 6,
    7, 8, 9
  ), 3, 3, byrow = TRUE)
  plot(nr, T)
  
}







