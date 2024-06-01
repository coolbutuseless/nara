
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert nativeRaster images to/from other R objects
#'
#' @param nr nativeRaster object
#' @param ras standard R raster i.e. a character matrix of hex colour values
#' @param arr 3d numeric array representing R,G,B,A values with dimensions [nrow, ncol, 4] or
#'        [nrow, ncol, 3]. Each value is in range [0,1].
#' @param dst destination nativeRaster. If NULL (the default) a new nativeRaster
#'        will be created.
#'
#' @return raster, array or nativeRaster
#' @import grDevices
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
#' Matrix to nativeRaster
#' 
#' @param mat integer matrix
#' @param palette vector of colours.  This palette must contain at least as 
#'        many colours as the maximum integer value in \code{mat}.
#' @param dst destination natriveRaster object. If NULL (the default) a 
#'        new one will be allocated.  If a ntiveRaster is supplied here, it 
#'        must have the exact dimensions to match the matrix        
#'        
#' @return nativeRaster
#' @export
#'
#' @examples
#' m <- matrix(1:12, 3, 4)
#' palette <- colour_to_integer(rainbow(12))
#' nr <- matrix_to_nr(m, palette) 
#' plot(nr)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
matrix_to_nr <- function(mat, palette, dst = NULL) {
  .Call(matrix_to_nr_, mat, colour_to_integer(palette), dst)
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

