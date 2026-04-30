
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
  library(insitu)
  
  w   <- 200
  h   <- 200
  fac <- 5
  
  obj <- rgl::icosahedron3d()
  ntri <- ncol(obj$it)
  set.seed(2)
  cols <- viridisLite::inferno(ntri) |> sample() # colors for triangles
  
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # flatten mesh data to pure vertex coordinate data
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  off <- w / 2
  vertices <- with(obj, vb[, obj$it])
  vertices <- (vertices * 40) + off
  
  pvertices <- vertices
  pvertices[] <- vertices
  
  nr <- nr_new(w, h)
  nr_tri_coords(nr, pvertices, cols)
  grid.raster(nr, interpolate = FALSE)
  
}



if (FALSE) {
  library(grid)
  library(insitu)
  library(naratigr)
  
  w   <- 200
  h   <- 200
  fac <- 5
  off <- w / 2
  
  # x11(width = w * fac / dpi, height = h * fac / dpi, antialias = 'none', type = 'dbcairo')
  # dev.control(displaylist = 'inhibit')
  window <- naratigr::tigr_open(w, h)
  
  
  # obj <- readobj::read.obj("working/obj/bunny.obj")
  # vertices <- obj$shapes[[1]]$positions[, obj$shapes[[1]]$indices]
  # vertices <- (vertices * 800) + off
  # ntri <- ncol(vertices) / 3
  
  # obj <- rgl::readOBJ("working/obj/newell_teaset/teapot.obj")
  obj <- rgl::readOBJ('working/obj/bunny2.obj')
  # obj <- rgl::addNormals(obj)
  # obj <- rgl::icosahedron3d()
  
  ntri <- ncol(obj$it)
  vertices <- with(obj, vb[, obj$it])
  vertices <- (vertices * 100) + off
  
  cols <- viridisLite::inferno(ntri) |> sample() # colors for triangles
  pvertices <- vertices
  pvertices[] <- vertices
  
  nr <- nr_new(w, h, fill = 'blue')
  tigr_update(window, nr)

  frame <- 1
  nframes <- 600
  start <- Sys.time()
  for (frame in seq_len(nframes)) {

    nr_fill(nr, 'white')
    nr_tri_coords(nr, pvertices, cols)
    # dev.hold()
    # grid.raster(nr, interpolate = FALSE)
    # dev.flush()
    tigr_update(window, nr)
    
    # The transformation function is dependent upon the frame number
    # in order to get a nice sinusoidal rotation effect
    frac <- (frame - 1)/nframes
    tf <- tf3_new() |> 
      tf3_add_translate(x = -off, y = -off, z = -off) |>
      tf3_add_rotate_x(2   * sin(2 * pi * frac + pi/4)) |>
      tf3_add_rotate_y(1   * sin(2 * pi * frac)) |>
      tf3_add_rotate_z(1.5 * sin(2 * pi * frac + pi/3)) |>
      # tf3_add_rotate_y(7 * nframes/300 * cos(frac * 2 + pi/4)) |>
      # tf3_add_rotate_z(3 * nframes/300 * sin(frac * 8 + pi/2)) |>
      tf3_add_translate(x = off, y = off, z = off)
    
    # Start with the raw vertex data and transform it, ready for the next frame
    br_copy(pvertices, vertices)
    pvertices |>
      br_mat_transpose() |>
      tf3_apply(tf) |>
      br_mat_transpose()
    
  }
  print(Sys.time() - start)
  
}



