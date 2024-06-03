

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
#' @param respect_alpha Should the alpha channel be respected when blitting?
#'        Default: TRUE.  If FALSE, then contents will be blindly overwritten
#'        which can be much much faster.  If the \code{src} has an 
#'        any transparent pixels, \code{respect_alpha = TRUE} is 
#'        probably the correct setting.
#'
#' @return \code{nativeRaster}
#' 
#' @examples
#' nr <- nr_new(50, 50, 'grey80')
#' nr_blit(nr, x = 1, y = 1, src = deer, x0 = 1, y0 = 129, w = 32, h = 32)
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit <- function(nr, x, y, src, x0 = 1L, y0 = 1L, w = NULL, h = NULL, respect_alpha = TRUE) {
  invisible(.Call(blit_, nr, x, y, src, x0, y0, w, h, respect_alpha))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Blit2
#' 
#' @inheritParams nr_blit
#' @param loc a 4 element numeric vector (or list) with the following values 
#'        in this exact order: (x0, y0, w, h) corresponding to the 
#'        separate arguments to \code{nr_blit()}
#'
#' @return \code{nativeRaster}
#' 
#' @examples
#' nr <- nr_new(50, 50, 'grey80')
#' nr_blit2(nr, x = 1, y = 1, src = deer, loc = deer_loc[1,])
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit2 <- function(nr, x, y, src, loc, respect_alpha = TRUE) {
  invisible(.Call(blit_, nr, x, y, src, loc[[1]], loc[[2]], loc[[3]], loc[[4]], respect_alpha))
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
  
 
  
  
  # 902us
  N <- 400
  nr <- nr_new(N, N, 'grey80')
  bench::mark(
    nr_blit2(nr, sample(seq(-16, N+16, length.out = N)), sample(N), deer, deer_loc[1,])
  )
  plot(nr, T)
  
   
  nr <- nr_new(5, 5, 'grey80'); 
  nr2 <- nr_new(3, 1, 'black')
  nr2[[1]] <- str_cols_to_packed_cols('red')
  nr2[[3]] <- str_cols_to_packed_cols('blue')
  nr_blit(nr, 3, -1, nr2); 
  plot(nr, T)
  
  
  nr <- nr_new(40, 40, 'grey80'); 
  nr_blit2(nr, -5, 1, deer, deer_loc[1,]); 
  nr_blit2(nr, -5, 1, deer, deer_loc[1,]); 
  plot(nr, T)
  
  
  # 500
  nr <- nr_new(1200, 800, 'grey80'); 
  # nr2 <- png::readPNG(system.file("img/Rlogo.png", package = "png"), native = TRUE)
  nr2 <- png::readPNG("~/Desktop/Screenshot 2024-05-31 at 1.23.16 PM.png", native = TRUE)
  dim(nr)
  dim(nr2)
  bench::mark(
    nr_blit(nr, -30, -10, nr2, respect_alpha = TRUE) 
  )
  plot(nr, T)
  # 9752 respect alpha = FALSE
  #  487 respect alpha = TRUE
  
  
}









