

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Copy one \code{nativeRaster} image into another at an arbitrary location.
#'
#' This is useful as a way of positioning sprites or icons in an image.
#'
#' @param nr native raster
#' @param sprite native raster to copy into \code{nr}
#' @param x,y Where in \code{nr} to place the \code{sprite}. These values must
#'        be vectors of the same length.  If the length is greater than 1, then
#'        the \code{sprite} will be pasted into \code{nr} at multiple locations.
#'        Note that the
#'        origin of \code{nativeraster} images is the bottom-left
#'        where the coordinates are (1, 1).
#'
#' @return Native raster
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit <- function(nr, sprite, x, y) {
  .Call(blit_, nr, sprite, x, y)
  invisible(nr)
}




if (FALSE) {

  library(grid)

  x11(type = 'dbcairo', width = 7, height = 3)
  dev.control('inhibit')


  nr <- nr_new(100, 30, 'grey80')
  nr_text(nr, "Hello #RStats", 'black', 0, 20)
  nr_blit(nr, dino[[1]], 2, 1)
  dev.hold()
  grid.raster(nr, interpolate = FALSE)
  dev.flush()

  start <- Sys.time()
  for (i in -30:110) {
    cat('.')
    nr_fill(nr, 'grey80')
    nr_text(nr, "Hello #RStats", 'black', 0, 20)
    nr_blit(nr, dino[[(i %% 16) + 1]], i, 1)
    dev.hold()
    grid.raster(nr, interpolate = FALSE)
    dev.flush()
    # Sys.sleep(0.06)
  }
  Sys.time() - start
}
