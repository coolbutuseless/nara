

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
#' nr_blit(nr, x = 0, y = 0, src = deer, x0 = 0, y0 = 0, w = 32, h = 32)
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit <- function(nr, x, y, src, x0 = 0L, y0 = 0L, w = NULL, h = NULL, respect_alpha = TRUE) {
  invisible(.Call(blit_, nr, x, y, src, x0, y0, w, h, respect_alpha))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Blit2
#' 
#' @inheritParams nr_blit
#' @param loc N x 4 matrix of multiple sprite locations within the \code{src}
#'        spritesheet
#' @param idx integer vector of rows within the \code{locs} matrix to use
#'
#' @return \code{nativeRaster}
#' 
#' @examples
#' nr <- nr_new(400, 400, 'grey80')
#' nr_blit2(nr, x = c(0, 100, 300), y = c(0, 100, 300), 
#'          src = deer, loc = deer_loc, idx = c(1, 4, 8))
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit2 <- function(nr, x, y, src, loc, idx = 1L, respect_alpha = TRUE) {
  
  if (!length(idx) %in% c(1, length(x))) {
    stop("R limitation. idx not currently recycled. Fix for 'C' implementation")
  }
  
  if (any(idx > nrow(loc))) {
    stop("Some idx out of bound: ", deparse1(idx))
  }
  
  if (length(idx) != length(x)) {
    idx <- rep(idx, length(x))
  }
  
  for (i in seq_along(idx)) {
    row <- idx[i]
    invisible(.Call(blit_, 
                    nr, x = x[i], y = y[i], 
                    src = src, 
                    x = loc[row, 1L], y = loc[row, 2L], w = loc[row, 3L], h = loc[row, 4L], 
                    respect_alpha = respect_alpha))
  }
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Blit4
#' 
#' @inheritParams nr_blit2
#' @param idx_mat integer matrix of indices into \code{loc}
#' @param width,height tile width/height (constant across all tiles)
#' 
#' @return Original \code{nativeRaster} modified in-place
#' 
#' @examples
#' nr <- nr_new(400, 400, 'grey80')
#' idx_mat <- matrix(c(1, 2, 3, 4, 5, 6), 3, 2)
#' nr_blit3(nr, x = 10, y = 20, 
#'          src = deer, loc = deer_loc, idx_mat = idx_mat, 
#'          width = 32, height = 32)
#' plot(nr)
#' 
#' @export 
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit3 <- function(nr, x, y, src, loc, idx_mat, width, height, respect_alpha = TRUE) {
  
  if (length(x) != 1 || length(y) != 1) {
    stop("only allow length 1 for x and y")
  }
  
  if (max(idx_mat, na.rm = TRUE) > nrow(loc)) {
    stop("loc idx is out of bounds")
  }
  
  stopifnot(is.matrix(idx_mat))
  
  xoff <- (rep(seq(ncol(idx_mat)), each = nrow(idx_mat)) - 1) * width
  yoff <- (rep(seq(nrow(idx_mat)),        ncol(idx_mat)) - 1) * height
  
  invisible(
    nr_blit2(nr, x = x + xoff, y = y + yoff, src = src, loc = loc, idx = as.vector(idx_mat))
  )
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

if (FALSE) {
  
  x = c(0, 100, 300)
  y = c(0, 100, 300)
  src <- deer
  loc <- deer_loc
  idx <- c(1, 4, 8)
  
  nr <- nr_new(400, 400, 'grey80')
  bench::mark(
    nr_blit2(nr, x = c(0, 100, 300), y = c(0, 100, 300), 
             src = deer, loc = deer_loc, idx = c(1, 4, 8))
  )
  plot(nr)
}

if (FALSE) {
  
  idx <- matrix(
    sample(seq_len(nrow(deer_loc)), 100, T), 10, 10
  )
  
  nrs <- lapply(0:32, function(size) {
    nr <- nr_new(320, 320, 'grey80')
    nr_blit3(nr, x = 0, y = 0, 
             src = deer, loc = deer_loc, idx_mat = idx, width = size, height = size)
    nr 
  })
  
  nrs_to_gif(nrs, "working/test2.gif")
  nrs_to_mp4(nrs, "working/test2.mp4")  
  
}









