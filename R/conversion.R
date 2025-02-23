
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
#' @param palette vector of colors.  For an integer matrix, this palette must 
#'        contain at least as many colors as the maximum integer value in \code{mat}. 
#'        For a numeric matrix, any length palette is allowed.
#' @param dst destination \code{nativeRaster} object. If NULL (the default) a 
#'        new \code{nativeRaster} will be created  If a \code{nativeRaster} 
#'        is supplied here, it must have the exact dimensions to match the matrix        
#' @param fill Color to be used for values < 1 when input is an integer matrix.  
#'        Default: 'transparent'.
#' @param min,max assumed range for the numeric data.  values from the palette
#'        will be interpolated using this range as the extents.  An error
#'        will occur if a value lies outside this range. Default: (0, 1)
#' @return \code{nativeRaster}
#'
#' @examples
#' m <- matrix(1:12, 3, 4)
#' palette <- colorfast::col_to_int(rainbow(12))
#' nr <- matrix_to_nr(m, palette) 
#' plot(nr)
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
matrix_to_nr <- function(mat, palette, fill = 'transparent', min = 0.0, max = 1.0, dst = NULL) {
  .Call(matrix_to_nr_, mat, palette, fill, min, max, dst)
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
  im <- system.file("image/deer-1.png", package = "nara", mustWork = TRUE)  
  arr <- fastpng::read_png(im, type = 'array')
  
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


if (FALSE) {
  mat <- matrix(seq(0, 10, length.out = 10)/ 10, 2, 5)
  
  palette <- rainbow(100) |> colorfast::col_to_int()
  matrix_to_nr(mat, palette = palette)
  
}






