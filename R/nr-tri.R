
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw multiple triangles from mesh data
#' 
#' @inheritParams nr_fill
#' @param vertices matrix of vertex coordinates. x,y in columns (wide) or 
#'        rows (tall).  Extra coordinates are ignored i.e. if coordiantes 
#'        given as (x, y, z) or (x, y, z, w), 
#'        only the first two coordinates will be used for plotting.
#' @param indices integer matrix of index information. 3 indices per triangle - 
#'        either in one-triangle-per-matrix_row (tall) or 
#'        one-triangle-per-matrix-coloumn (wide)
#' @param coords direct coordinate data for each triangle.  Can be 'tall' or 'wide'
#' @param col color specification. Single color or one color per tri
#' @param format format of data. i.e. 'tall', 'wide' or 'auto'. Default: 'auto'
#' @param draw_all adjust all triangles to be the correct orientation
#' 
#' @return Invisibly return native raster
#' @family drawing functions
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_tri_mesh <- function(nr, vertices, indices, col, draw_all = FALSE, format = 'auto') {
  stop("nr_tri_mesh(): Not done yet")
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_tri_mesh
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_tri_coords <- function(nr, coords, col, draw_all = FALSE, format = 'auto') {
  invisible(
    .Call(nr_tri_coords_, nr, coords, col, draw_all, format)
  )
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw an outline wireframe mesh only
#' 
#' @inheritParams nr_tri_mesh
#' @return nr
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_wireframe_mesh <- function(nr, vertices, indices, col, draw_all = FALSE, format = 'auto') {
  stop("nr_wireframe_mesh(): not done yet")
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_wireframe_mesh
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_wireframe_coords <- function(nr, coords, col, draw_all = FALSE, format = 'auto') {
  stop("nr_wireframe_coords(): not done yet")
}



if (FALSE) {
  library(grid)
  
  w   <- 200
  h   <- 200
  fac <- 5
  
  obj <- rgl::icosahedron3d()
  
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # flatten mesh data to pure vertex coordinate data
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  off <- w / 2
  coords <- with(obj, vb[, obj$it])
  coords <- (coords * 60) + off
  
  nr <- nr_new(w, h)
  nr_tri_coords(nr, coords, 'red')
  grid.raster(nr, interpolate = FALSE)
  
  
}



