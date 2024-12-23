

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Copy one \code{nativeRaster} image into another at an arbitrary location.
#'
#' This is useful as a way of positioning sprites or icons in an image.
#'
#' @param dst,src source and destination native rasters
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
#'        Default: TRUE means to carefully blend pixels at each location 
#'        using alpha values.  If FALSE, then contents at the \code{dst} will 
#'        just be replaced with \code{src} pixels
#'        which can be much much faster.
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
#' nr_blit(dst = nr, src = deer_sprites[[1]], x = 0, y = 0)
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit <- function(dst, src, 
                    x, y, 
                    x0    =  0L , y0    =  0L, 
                    w     = -1L , h     = -1L, 
                    hjust =  0  , vjust =  0, 
                    respect_alpha = TRUE) {
  
  
  invisible(.Call(blit_, 
                  dst  , src, 
                  x    , y, 
                  x0   , y0, 
                  w    , h, 
                  hjust, vjust, 
                  respect_alpha))
}




if (FALSE) {
  
  logo <- fastpng::read_png(system.file("img", "Rlogo.png", package="png"), type = 'nativeraster')
  
  nr <- nr_new(200, 200, 'grey80')
  
  nr_blit(nr, logo, 0, 0, hjust = 0.5, vjust = 0.5, respect_alpha = F)
  
  plot(nr, T)
  
}







