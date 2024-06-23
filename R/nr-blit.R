

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


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Blit from a list of native rasters
#' 
#' @inheritParams nr_blit
#' @param src_list list of native rasters
#' @param src_idx indices into the list of the native raster
#' 
#' @examples
#' nr <- nr_new(50, 50, 'grey80')
#' nr_blit_list(nr, x = c(0, 25), y = c(0, 25), src_list = deer_sprites, src_idx = c(1, 2))
#' plot(nr)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit_list <- function(nr, x, y, src_list, src_idx, hjust = 0, vjust = 0, respect_alpha = TRUE) {
  invisible(.Call(blit_list_, nr, x, y, src_list, src_idx, hjust, vjust, respect_alpha))
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Blit4
#' 
#' @inheritParams nr_blit_list
#' @param idx_mat integer matrix of indices into \code{src_list}
#' @param width,height tile width/height (constant across all tiles)
#' 
#' @return Original \code{nativeRaster} modified in-place
#' @examples
#' nr <- nr_new(100, 100, 'grey80')
#' idx_mat <- matrix(c(
#'   1, 2, 3,
#'   4, 5, 6,
#'   7, 8, 9
#' ), 3, 3, byrow = TRUE)
#' nr_blit_grid(nr, 0, 0, src_list = deer_sprites, idx_mat = idx_mat, width = 32, height = 32)
#' 
#' @export 
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit_grid <- function(nr, x, y, src_list, idx_mat, width, height, hjust = 0, vjust = 0, respect_alpha = TRUE) {
  
  if (length(x) != 1 || length(y) != 1) {
    stop("only allow length 1 for x and y")
  }
  
  if (max(idx_mat, na.rm = TRUE) > length(src_list)) {
    stop("loc idx is out of bounds")
  }
  
  stopifnot(is.matrix(idx_mat))
  
  xoff <- (rep(seq(ncol(idx_mat)), each = nrow(idx_mat)) - 1) * width
  yoff <- (rep(seq(nrow(idx_mat)),        ncol(idx_mat)) - 1) * height
  
  invisible(
    nr_blit_list(nr, x = x + xoff, y = y + yoff, src_list = src_list, src_idx = as.vector(idx_mat), 
             hjust = hjust, vjust = vjust, respect_alpha = respect_alpha)
  )
}





if (FALSE) {
  
  logo <- png::readPNG(system.file("img", "Rlogo.png", package="png"), native = TRUE)
  
  nr <- nr_new(100, 100, 'grey80')
  idx_mat <- matrix(c(
    1, 2, 3,
    4, 5, 6,
    7, 8, 9
  ), 3, 3, byrow = TRUE)
  nr_blit_grid(nr, 0, 0, src_list = deer_sprites, idx_mat = idx_mat, width = 32, height = 32)
  plot(nr, T)
  
}







