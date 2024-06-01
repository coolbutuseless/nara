

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
  ras <- integer_to_colour((nr))
  dim(ras) <- dim(nr)
  class(ras) <- 'raster'
  ras
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_to_raster
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
raster_to_nr <- function(ras, dst = NULL) {
  
  if (FALSE) {
    nr <- matrix(colour_to_integer(t(ras)), nrow = nrow(ras), ncol = ncol(ras))
    class(nr) <- 'nativeRaster'
    attr(nr, 'channels') <- 4L
    nr
  }
  
  .Call(raster_to_nr_, ras, dst)
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



if (FALSE) {
  
  m <- matrix(1:12, 3, 4)
  palette <- colour_to_integer(rainbow(12))
  matrix_to_nr(m, palette) |> plot(T)

  scales::show_col(rainbow(12))  
}


if (FALSE) {
  n_colours <- 5
  my_palette <- hsv(seq(0, 1, length.out = n_colours))
  w <- 200
  h <- 100

  m <- matrix(data = sample(seq_len(n_colours), size = w * h, replace = TRUE), nrow = h)
  
  dst <- nr_new(w, h);
  
  bench::mark(
    {matrix(my_palette[m], nrow = h) |> raster_to_nr()},
    matrix_to_nr(m, my_palette),
    matrix_to_nr(m, my_palette, dst)
  )
  
  
  for (i in 1:20) {
    m2 <- matrix(my_palette[m], nrow = h)
    nr <- raster_to_nr(m2)
    grid.raster(nr, interpolate = FALSE)
    Sys.sleep(0.1)
  }
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
