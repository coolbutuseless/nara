

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
#'        origin of \code{nativeraster} images is the bottom-left
#'        where the coordinates are (1, 1).
#' @param x0,y0 start coordiates within src
#' @param w,h size within src
#'
#' @return Native raster
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit <- function(nr, x, y, src, x0 = 1L, y0 = 1L, w = NULL, h = NULL) {
  .Call(blit_, nr, x, y, src, x0, y0, w, h)
  invisible(nr)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Blit2
#' 
#' @inheritParams nr_blit
#' @param loc a 4 element numeric vector (or list) with the following values 
#'        in this exact order: (x0, y0, w, h) corresponding to the 
#'        separate arguments to \code{nr_blit()}
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit2 <- function(nr, x, y, src, loc) {
  .Call(blit_, nr, x, y, src, loc[[1]], loc[[2]], loc[[3]], loc[[4]])
  invisible(nr)
}


if (FALSE) {
  library(grid)

  x11(type = 'dbcairo', width = 7, height = 3)
  dev.control('inhibit')


  nr <- nr_new(100, 30, 'grey80')
  nr_text(nr, "Hello #RStats", 0, 20, 'black')
  nr_blit(nr, 2, 1, src = dino[[1]])
  dev.hold()
  grid.raster(nr, interpolate = FALSE)
  dev.flush()

  start <- Sys.time()
  for (i in -30:110) {
    cat('.')
    nr_fill(nr, 'grey80')
    nr_text(nr, "Hello #RStats", 0, 20, 'black')
    nr_blit(nr, i, 1, dino[[(i %% 16) + 1]])
    dev.hold()
    grid.raster(nr, interpolate = FALSE)
    dev.flush()
    # Sys.sleep(0.06)
  }
  Sys.time() - start
}



if (FALSE) {
  source <- "working/dino-spritesheet.png"
  ss <- read_png(source, transparent = list(-1594494))
  if (FALSE) {
    grid.newpage(); grid.raster(ss)
  }
  
  nr <- nr_new(100, 100, 'grey90') 
  nr_rect(nr, 1, 1, 28, 25)
  nr_blit(nr, 1, 1, ss, 7, 5, 26, 25)
  grid.newpage(); grid.raster(nr, interpolate = FALSE)
  
   # nr_rect(nr, 1, 1, 30, 24)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(  7, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c( 40, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c( 74, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(107, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(141, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(174, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(204, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(238, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(270, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(301, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(333, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(365, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(398, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(429, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(461, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
}












