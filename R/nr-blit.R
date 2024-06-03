

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


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Blit3
#' 
#' @inheritParams nr_blit
#' @param loc_mat N x 4 matrix of multiple sprite locations within the \code{src}
#'        spritesheet
#' @param loc_idx integer vector of rows within the \code{locs} matrix to use
#' 
#' @return Original \code{nativeRaster} modified in-place
#' 
#' @examples
#' nr <- nr_new(400, 400, 'grey80')
#' nr_blit3(nr, x = c(1, 100, 300), y = c(1, 100, 300), 
#'          src = deer, loc_mat = deer_loc, loc_idx = c(1, 4, 8))
#' plot(nr)
#' 
#' @export 
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit3 <- function(nr, x, y, src, loc_mat, loc_idx, respect_alpha = TRUE) {
  if (!length(loc_idx) %in% c(1, length(x))) {
    stop("R limitation. loc_idx not currently recycled. Fix for 'C' implementation")
  }
  
  for (i in seq_along(loc_idx)) {
    nr_blit2(nr, x = x[i], y = y[i], src = src, loc = loc_mat[loc_idx[i],], respect_alpha = respect_alpha)
  }
  
  invisible(nr)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Blit4
#' 
#' @inheritParams nr_blit3
#' @param idx_mat matrix of indices into \code{loc_mat}
#' @param width,height tile width/height (constant across all tiles)
#' 
#' @return Original \code{nativeRaster} modified in-place
#' 
#' @examples
#' nr <- nr_new(400, 400, 'grey80')
#' idx_mat <- matrix(c(1, 2, 3, 4, 5, 6), 3, 2)
#' nr_blit4(nr, x = 10, y = 20, 
#'          src = deer, loc_mat = deer_loc, idx_mat = idx_mat, 
#'          width = 32, height = 32)
#' plot(nr)
#' 
#' @export 
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit4 <- function(nr, x, y, src, loc_mat, idx_mat, width, height, respect_alpha = TRUE) {
  
  if (length(x) != 1 || length(y) != 1) {
    stop("length 1 for (x,y) only")
  }
  
  if (max(idx_mat, na.rm = TRUE) > nrow(loc_mat)) {
    stop("loc idx is out of bounds")
  }
  
  xoff <- (rep(seq(ncol(idx_mat)), each = nrow(idx_mat)) - 1) * width
  yoff <- (rep(seq(nrow(idx_mat)),        ncol(idx_mat)) - 1) * height
  
  nr_blit3(nr, x = x + xoff, y = y + yoff, src = src, loc_mat = loc_mat, loc_idx = as.vector(idx_mat))
  
  
}


if (FALSE) {
  library(grid)
  
  nr <- nr_new(400, 400, 'grey80')
  idx_mat <- matrix(c(1, 14, 3, 4, 5, 6), 3, 2)
  xoff <- (rep(seq(ncol(idx_mat)), each = nrow(idx_mat)) - 1) * width
  yoff <- (rep(seq(nrow(idx_mat)),        ncol(idx_mat)) - 1) * height
  
  x <- 1
  y <- 1
  
  # nr_blit3(nr, x = x + xoff, y = y + yoff, src = deer, loc_mat = deer_loc, loc_idx = as.vector(idx_mat))
  
  nr_blit4(nr, x = 20, y = 90, src = deer, loc_mat = deer_loc, idx_mat = idx_mat, width = 32, height = 32)
  plot(nr)
  
  
}


