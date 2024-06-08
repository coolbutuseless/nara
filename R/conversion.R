
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert \code{nativeRaster} images to/from other R objects
#'
#' @param nr \code{nativeRaster} object
#' @param ras standard R raster i.e. a character matrix of hex color values
#' @param arr 3d numeric array representing R,G,B,A values with dimensions [nrow, ncol, 4] or
#'        [nrow, ncol, 3]. Each value is in range [0,1].
#' @param dst destination \code{nativeRaster} If NULL (the default) a new \code{nativeRaster}
#'        will be created.
#'
#' @return raster, array or \code{nativeRaster}
#' 
#' @examples
#' nr <- nr_new(12, 8, 'hotpink')
#' nr_to_raster(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_to_raster <- function(nr) {
  .Call(nr_to_raster_, nr)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_to_raster
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
raster_to_nr <- function(ras, dst = NULL) {
  .Call(raster_to_nr_, ras, dst)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_to_raster
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_to_array <- function(nr) {
  .Call(nr_to_array_, nr)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_to_raster
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
array_to_nr <- function(arr, dst = NULL) {
  .Call(array_to_nr_, arr, dst)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Matrix to \code{nativeRaster}
#' 
#' @param mat integer matrix
#' @param palette vector of colors.  This palette must contain at least as 
#'        many colors as the maximum integer value in \code{mat}. 
#' @param dst destination \code{nativeRaster} object. If NULL (the default) a 
#'        new \code{nativeRaster} will be created  If a \code{nativeRaster} 
#'        is supplied here, it must have the exact dimensions to match the matrix        
#' @param fill Color to be used for values < 1.  Default: 'transparent'        
#' @return \code{nativeRaster}
#'
#' @examples
#' m <- matrix(1:12, 3, 4)
#' palette <- str_cols_to_packed_cols(rainbow(12))
#' nr <- matrix_to_nr(m, palette) 
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
matrix_to_nr <- function(mat, palette, fill = 'transparent', dst = NULL) {
  .Call(matrix_to_nr_, mat, palette, fill, dst)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert a 'magick' image to \code{nativeRaster}
#' 
#' @inheritParams matrix_to_nr
#' @inheritParams array_to_nr
#' @param im image from the \code{magick} package
#' 
#' @return \code{nativeRaster}
#' 
#' @examples
#' if (requireNamespace('magick', quietly = TRUE)) {
#'   im <- magick::image_read(system.file("img/Rlogo.png", package = "png"))
#'   nr <- magick_to_nr(im)
#'   plot(nr)
#' }
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
magick_to_nr <- function(im, dst = NULL) {
  if (requireNamespace('magick', quietly = TRUE)) {
    .Call(magick_to_nr_, magick::image_data(im), dst)
  } else {
    message("Please install the 'magick' package in order to use this function");
    NULL
  }
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname magick_to_nr
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_to_magick <- function(nr) {
  if (requireNamespace('magick', quietly = TRUE)) {
    magick::image_read(nr)
  } else {
    message("Please install the 'magick' package in order to use this function");
    NULL
  }
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Inline visual testing
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if (FALSE) {
  
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Matrix with palette
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  m <- matrix(1:12, 3, 4)
  palette <- rainbow(12)
  matrix_to_nr(m, palette) |> plot()

  m[] <- palette[m]
  plot(as.raster(m), interpolate = F)
  
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Array
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  im <- system.file("img/Rlogo.png", package = "png", mustWork = TRUE)  
  arr <- png::readPNG(im)
  
  plot(as.raster(arr), interpolate = FALSE)
  nr <- array_to_nr(arr)
  plot(nr, T)  
  arr2 <- nr_to_array(nr)
  identical(dim(arr), dim(arr2))
  identical(unclass(arr), unclass(arr2))
  
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Raster
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ras <- as.raster(arr)
  plot(ras, interpolate = FALSE)
  
  nr <- raster_to_nr(ras)
  plot(nr, T)

  ras2 <- nr_to_raster(nr)
  plot(ras2, interpolate = FALSE)

  identical(dim(ras), dim(ras2))
  identical(unclass(ras), toupper(unclass(ras2)))
}

