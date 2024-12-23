

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Copy one \code{nativeRaster} image into another at an arbitrary location.
#'
#' Single source blitted to one or more locations.
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
#' @return None. \code{dst} modifief by-reference and returned invisibly.
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
  
  
  invisible(.Call(nr_blit_, 
                  dst  , src, 
                  x    , y, 
                  x0   , y0, 
                  w    , h, 
                  hjust, vjust, 
                  respect_alpha))
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Multiple blit operations in a single call
#' 
#' @param dst destination native raster
#' @param src list of native rasters
#' @param config data.frame of configuration information for each blit which 
#'    most contain: idx, x, y, x0, y0, w, h, hjust, vjust, respect_alpha, draw
#' @return None. \code{dst} modifief by-reference and returned invisibly.
#' @examples
#' nr <- nr_new(90, 90, 'grey60')
#' config <- data.frame(
#'   idx = c(1, 2, 3, 4),
#'   x = c(10, 10, 40, 40),
#'   y = c(10, 40, 40, 10),
#'   x0 = 0L,
#'   y0 = 0L,
#'   w = -1L,
#'   h = -1L,
#'   hjust = 0,
#'   vjust = 0,
#'   respect_alpha = TRUE,
#'   render = TRUE
#' )
#' nr_blit_bulk(dst = nr, src = deer_sprites, config = config)
#' plot(nr)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit_bulk <- function(dst, src, config) {
  invisible(
    .Call(nr_blit_bulk_, dst, src, config)
  )
}




if (FALSE) {
  
  logo <- fastpng::read_png(system.file("img", "Rlogo.png", package="png"), type = 'nativeraster')
  nr <- nr_new(200, 200, 'grey80')
  nr_blit(nr, logo, 0, 0, hjust = 0.5, vjust = 0.5, respect_alpha = F)
  plot(nr, T)
  
  
  
  library(grid)
  
  # Setup a fast graphics device that can render quickly
  x11(type = 'cairo', antialias = 'none')
  dev.control('inhibit')
  
  logo <- fastpng::read_png(system.file("img", "Rlogo.png", package="png"), type = 'nativeraster')
  logo <- nr_scale(logo, 2, algo = 'bilinear')
  grid <- expand.grid(x = seq(-500, 1000, 220), y = seq(-500, 1000, 200))
  nr <- nr_new(500, 500, 'white')
  
  gc(full = TRUE)
  dev.flush()
  dev.flush()
  dev.flush()
  gov <- governor::gov_init(1/30)
  for (i in 1:500) {
    xoff <- 200 * sin(i/10 * 1/4 * pi)
    yoff <- 130 * sin(i/13 * 1/4 * pi + pi / 3)
    nr_fill(nr, 'white')
    nr_blit(nr, logo, grid$x - xoff , grid$y - yoff, hjust = 0.5, vjust = 0.5, respect_alpha = T)
    dev.hold()
    plot(nr)
    dev.flush()
    governor::gov_wait(gov)
  }
}


if (FALSE) {
  
}







