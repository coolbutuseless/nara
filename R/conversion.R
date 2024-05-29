

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert nativeRaster images to/from other R objects
#'
#' @param nr nativeRaster object
#' @param ras standard R raster i.e. a character matrix of hex colour values
#' @param arr 3d numeric array representing R,G,B,A values with dimensions [nrow, ncol, 4] or
#'        [nrow, ncol, 3]. Each value is in range [0,1].
#'
#' @return raster, array or nativeRaster
#' @import grDevices
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_to_raster <- function(nr) {
  ras <- integer_to_colour((nr))
  dim(ras) <- rev(dim(nr))
  ras <- t(ras)

  ras
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_to_raster
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
raster_to_nr <- function(ras) {
  nr <- matrix(colour_to_integer(t(ras)), nrow = nrow(ras), ncol = ncol(ras))
  class(nr) <- 'nativeRaster'
  attr(nr, 'channels') <- 4L
  nr
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_to_raster
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_to_array <- function(nr) {
  ras  <- nr_to_raster(nr)
  rgba <- col2rgb(ras, alpha = TRUE) / 255
  arr  <- array(t(rgba), c(dim(ras), 4))

  arr
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_to_raster
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
array_to_nr <- function(arr) {

  # Sanity check
  stopifnot(length(dim(arr)) == 3)

  # Need to add an alpha channel?
  if (dim(arr)[3] == 3) {
    alpha <- rep(1, prod(dim(arr)[1:2]))
    new_dim <- dim(arr)
    new_dim[3] <- 4
    values <- c(arr, alpha)
    dim(values) <- new_dim
    arr <- values
  }

  stopifnot(dim(arr)[3] == 4)

  # Tranpose rows/cols of array
  arr <- aperm(arr, c(2, 1, 3))

  # Calculate colours
  cols <- grDevices::rgb(arr[,,1], arr[,,2], arr[,,3], arr[,,4])
  nr   <- colour_to_integer(cols)
  dim(nr) <- dim(arr)[2:1]

  # Give it class attributes
  class(nr) <- 'nativeRaster'
  attr(nr, 'channels') <- 4L

  nr
}


if (FALSE) {
  library(grid)

  ras <- nr_to_raster(dino[[2]])
  grid.newpage(); grid.raster(ras, interpolate = FALSE)

  nr <- raster_to_nr(ras)
  grid.newpage(); grid.raster(nr, interpolate = FALSE)


  arr <- png::readPNG("working/Rlogo-tiny.png")
  nr <- array_to_nr(arr)
  grid.newpage(); grid.raster(nr, interpolate = FALSE)


  ras <- nr_to_raster(nr)
  rgba <- col2rgb(ras, alpha = TRUE) / 255
  arr2 <- array(t(rgba), c(dim(ras), 4))


}
