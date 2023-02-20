

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Add isocubes
#'
#' @param nr nativeRaster
#' @param x,y coords
#' @param ysize cube face height (in pixels)
#' @param fill_top,fill_left,fill_right colours for top, left and right
#'        faces respectively. Can be single colours or vectors.
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_isocube <- function(nr, x, y, ysize, fill_top, fill_left, fill_right) {
  invisible(.Call(isocube_, nr, x, y, ysize, fill_top, fill_left, fill_right))
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Cheap version of darkening a colour (but not at all nice)
#'
#' @param fill vector of R colours
#' @param amount fraction to darken by
#'
#' @importFrom grDevices rgb col2rgb
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
darken <- function(fill, amount) {
  mat <- col2rgb(fill, alpha = TRUE)
  mat[1:3,] <- mat[1:3,] * (1 - amount)
  rgb(mat[1,], mat[2,], mat[3,], mat[4,], maxColorValue = 255)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Determine which cubes are visible
#'
#'
#' @param coords integer coordinates of isocubes positions. This function
#'        assumes that coordintates have already sorted from front to back.
#'
#' @return logical vector indicating which cubes are visible
#'
#' @noRd
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
visible_cubes <- function(coords) {

  rev(
    !duplicated(
      rev((coords$x - coords$z) * 1024L + ((coords$x + coords$z) * 0.5 + coords$y))
    )
  )

}




#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Create a grob of isocubes
#'
#' @param nr \code{nativeRaster} object
#' @param coords data.frame of x,y,z coordinates for the cubes (integer coordinates)
#' @param fill fill colour for the main face of cube as specified by the \code{light}
#'        argument.  By default, this will be the colour of the top face of the cube,
#'        as the default \code{light = 'top-left'}.
#' @param fill2,fill3 fill colours for secondary and tertiary faces of
#'        cube - as specified in the \code{light} argument.
#'        If set to NULL (the default) then cube faces will be darkened versions
#'        of the main \code{fill} color.
#' @param light direction of light.  This is a two-word argument nominating the
#'        main light direction and secondary light direction. The default value of
#'        'top-left' indicates the light is brightest from the top, and then
#'        from the left side of the cube - with the right side of the cube being darkest.
#'        This argument is only used if \code{fill2} and \code{fill3} are unspecified.
#'        Possible values are: top-left, top-right, left-top, left-right, right-top,
#'        right-left.
#' @param ysize the height in pixels of each cube face
#' @param xo,yo the origin of the isometric coordinate system in 'pixel' coordinates.
#'        These values should be given as integer pixel coordinates.
#'        Be default the origin is the middle bottom of the graphics device
#'        i.e. \code{(xo, yo) = (im_width/2, 1)}
#' @param verbose Be verbose? Default FALSE.
#' @param bg_fill background fill for raster image. Default: white
#'
#' @import grid
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_isocube3d <- function(nr, coords, fill = 'grey90', fill2 = NULL, fill3 = NULL,
                         light = 'top-left',
                         ysize,
                         xo = im_width/2,
                         yo = 0,
                         verbose = FALSE, bg_fill = 'white') {

  if (nrow(coords) == 0) {
    return(invisible(nr))
  }

  # depth sort the cubes
  coords$x <- as.integer(round(coords$x))
  coords$y <- as.integer(round(coords$y))
  coords$z <- as.integer(round(coords$z))

  sort_order <- with(coords, order(-x, -z, y))
  coords     <- coords[sort_order,]

  # Keep only cubes which are visible
  Norig   <- nrow(coords)
  visible <- visible_cubes(coords)
  if (verbose) message("Visible cubes: ", sum(visible), " / ", nrow(coords))
  coords  <- coords[visible,]



  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Prepare the fill colours
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (length(fill) == 1) {
    fill <- rep(fill, Norig)
  } else if (length(fill) != Norig) {
    stop("'fill' must be length = 1 or N")
  }

  # Rearrange colours to match depth-sorted cubes
  fill <- fill[sort_order]
  fill <- fill[visible]
  N    <- nrow(coords)

  if (is.null(fill2)) {
    fill2 <- darken(fill, 0.3)
  } else {
    if (length(fill2) == 1) {
      fill2 <- rep(fill2, Norig)
    } else if (length(fill2) != Norig) {
      stop("'fill2' must be length = 1 or N")
    }
    fill2 <- fill2[sort_order]
    fill2 <- fill2[visible]
  }

  if (is.null(fill3)) {
    fill3 <- darken(fill, 0.6)
  } else {
    if (length(fill3) == 1) {
      fill3 <- rep(fill3, Norig)
    } else if (length(fill3) != Norig) {
      stop("'fill3' must be length = 1 or N")
    }
    fill3 <- fill3[sort_order]
    fill3 <- fill3[visible]
  }


  im_width  <- ncol(nr)
  im_height <- nrow(nr)


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Flatten 3d coordinates to 2d space
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ix <- (coords$x - coords$z) * ysize + xo
  iy <- (coords$x + coords$z) * ysize * 0.5 + coords$y * ysize + yo


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Render to nativeRaster
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  colors <- switch (
    light,
    'top-left'   = nr_isocube(nr, ix, iy, ysize, fill_top = fill , fill_left = fill2, fill_right = fill3),
    'top-right'  = nr_isocube(nr, ix, iy, ysize, fill_top = fill , fill_left = fill3, fill_right = fill2),
    'left-top'   = nr_isocube(nr, ix, iy, ysize, fill_top = fill2, fill_left = fill , fill_right = fill3),
    'left-right' = nr_isocube(nr, ix, iy, ysize, fill_top = fill3, fill_left = fill , fill_right = fill2),
    'right-top'  = nr_isocube(nr, ix, iy, ysize, fill_top = fill2, fill_left = fill3, fill_right = fill ),
    'right-left' = nr_isocube(nr, ix, iy, ysize, fill_top = fill3, fill_left = fill2, fill_right = fill ),
    stop("'light' argument is not valid: ", light)
  )


  nr_isocube(nr, ix, iy, ysize, fill_top = fill, fill_left = fill2, fill_right = fill3)


  invisible(nr)
}




if (FALSE) {
  library(grid)
  nr <- nr_new(60, 60)
  nr_isocube(nr, 1, 1, 20, 'red', 'green', 'blue')
  grid.newpage(); grid.raster(nr, interpolate = FALSE)
}


if (FALSE) {
  library(grid)
  width <- 1000
  height <- 800
  N <- 50
  nr <- nr_new(width, height)
  # bench::mark(a={
  nr_isocube(nr,
             x = runif(N, 1, width),
             y = runif(N, 1, height),
             ysize = sample(30:90, N, replace = TRUE),
             fill_top = rainbow(N),
             fill_left = rev(rainbow(N)),
             fill_right = viridisLite::cividis(N))
  # })
  grid.newpage(); grid.raster(nr, interpolate = FALSE)
}


if (FALSE) {

  library(grid)

  x11(type = 'cairo', antialias = 'none')
  dev.control('inhibit')


  nr <- nr_new(800, 640, fill = 'grey80')
  N <- 20L
  coords <- expand.grid(x=seq(1L, N, 1L), y=seq(1L, N, 1L), z=seq(1L, N, 1L))
  nrow(coords)
  sum(visible_cubes(coords, 1))
  sum(visible_cubes(coords, 2))
  sum(visible_cubes(coords, 3))

  sum(visible_cubes_new(coords))

  bench::mark(
    visible_cubes(coords, 3),
    visible_cubes_new(coords),
    check = FALSE
  )

  nr_isocube3d(nr, coords, ysize = 30, fill = rainbow(nrow(coords)))
  grid.newpage(); grid.raster(nr, interpolate = FALSE); dev.flush()

}




