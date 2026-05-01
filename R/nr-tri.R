
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw multiple triangles from mesh data
#' 
#' @inheritParams nr_fill
#' @inheritParams nr_rect
#' @param vertices matrix of vertex coordinates. x,y in columns (wide) or 
#'        rows (tall).  Extra coordinates are ignored i.e. if coordiantes 
#'        given as (x, y, z) or (x, y, z, w), 
#'        only the first two coordinates will be used for plotting.
#' @param indices integer matrix of index information. 3 indices per triangle - 
#'        either in one-triangle-per-matrix_row (tall) or 
#'        one-triangle-per-matrix-column (wide)
#' @param coords Numeric atrix of direct coordinate data for each triangle.  
#'        The coordinates for
#'        each vertex are in columns. Each group of 3 columns defines one
#'        triangle.  There may be extra rows in the 
#'        matrix, but only the first two will be used (as x and y 
#'        respectively)
#' @param color color specification. Single color or one color per tri
#' @param tris Which triangles should be drawn?  Valid options: 'all' (default), 
#'        'ccw', 'cw'. The options 'ccw' and 'cw' limit plotting to those triangles
#'        where the order of the vertices are defined in a counter-clockwise
#'        or clockwise manner respectively.  Note: when considering orientation
#'        or triangles remmeber that the y-axis is defined vertically \emph{down}
#'        the screen. 
#' 
#' @examples
#' #' Using direct coordinates
#' set.seed(1)
#' w <- 100
#' h <-  80
#' nr <- nr_new(w, h)
#' 
#' n_tri <- 10
#' xs <- runif(n_tri * 3, 0, w - 1)
#' ys <- runif(n_tri * 3, 0, h - 1)
#' coords <- rbind(xs, ys)
#' cols <- rainbow(n_tri)
#' 
#' nr_tri_coords(nr, coords, cols, tris = 'all')
#' grid::grid.newpage()
#' grid::grid.raster(nr, interpolate = FALSE)
#' 
#' # Using a standard mesh structure
#' # i.e. a matrix of vertices, and a matrix of indices 
#' nr <- nr_new(w, h)
#' 
#' # Matrix of coordinates
#' xs <- rep(seq(0, w - 1, length.out = 10), 10)
#' ys <- rep(seq(0, h - 1, length.out = 10), each = 10)
#' vertices <- rbind(xs, ys)
#' nr_point(nr, xs, ys)
#' 
#' grid::grid.newpage()
#' grid::grid.raster(nr, interpolate = FALSE)
#' 
#' # Matrix of idices indicating which vertices make up each triangle
#' indices <- matrix(sample(length(xs), 3 * n_tri), nrow = 3) 
#' 
#' nr_tri_mesh(nr, vertices, indices, cols, tris = 'all')
#' grid::grid.newpage()
#' grid::grid.raster(nr, interpolate = FALSE)
#' 
#' @return Invisibly return the supplied native raster image which was been modified in-place
#' @family drawing functions
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_tri_mesh <- function(nr, vertices, indices, color, tris = 'all', mode = draw_mode$respect_alpha) {
  invisible(
    .Call(nr_tri_mesh_, nr, vertices, indices, color, tris, mode)
  )
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_tri_mesh
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_tri_coords <- function(nr, coords, color, tris = 'all', mode = draw_mode$respect_alpha) {
  invisible(
    .Call(nr_tri_coords_, nr, coords, color, tris, mode)
  )
}


if (FALSE) {
  library(grid)
  library(insitu)
  library(naratigr)
  
  w   <- 200
  h   <- 200
  fac <- 5
  window <- naratigr::tigr_open(w, h)
  
  obj <- rgl::icosahedron3d()
  obj <- rgl::readOBJ("working/obj/newell_teaset/teapot.obj")
  # obj <- rgl::readOBJ('working/obj/bunny2.obj')
  # obj <- rgl::readOBJ("~/projectsdata/obj/bunny.obj")
  obj <- rgl::readOBJ("~/projectsdata/obj/stanford-bunny.obj")
  obj$vb[2,] <- obj$vb[2,] - 0.1
  
  obj$vb <- obj$vb * 1000 + w/2
  ntri <- ncol(obj$it)
  cols <- viridisLite::magma(ntri)
  
  nr <- nr_new(w, h)
  nr_fill(nr, 'black')
  nr_tri_mesh(nr, obj$vb, obj$it, cols, tris = 'ccw')
  tigr_update(window, nr)
  
  
  
  tigr_close(window)
}



if (FALSE) {
  library(grid)
  library(insitu)
  library(naratigr)
  
  w   <- 200
  h   <- 200
  fac <- 5
  
  obj <- rgl::icosahedron3d()
  ntri <- ncol(obj$it)
  window <- naratigr::tigr_open(w, h)
  
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
  
  nframes <- 300
  start <- Sys.time()
  for (frame in seq_len(nframes)) {
    
    nr_fill(nr, 'black')
    nr_tri_coords(nr, pvertices, cols, tris = 'ccw')
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
  
  
  tigr_close(window)
  
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
  # obj <- rgl::readOBJ('working/obj/bunny2.obj')
  obj <- rgl::readOBJ("~/projectsdata/obj/bunny.obj")
  # obj <- rgl::readOBJ("~/projectsdata/obj/stanford-bunny.obj")
  obj <- rgl::addNormals(obj)
  obj |> lobstr::obj_size()
  
  {
    # Get centroid (or at least one point on tri)
    
    # Set light point (1000, 10000, 10000)
    # Calculate vector from light to centroid
    # Calcualte dot produce (light.vector x normal)
    # Use this to create a shaded colour
    i <- obj$it[1,]
    centroids <- obj$vb[1:3, i]
    to_light  <- c(2, 2, 2) - centroids
    
    mag <- sqrt(colSums(to_light ^ 2))
    mag <- rbind(mag, mag, mag)
    to_light <- to_light / mag
    
    vert_norm <- obj$normals[1:3,]
    face_norm <- vert_norm[,i]
    dotprod <- colSums(to_light * face_norm)
    
    dotprod <- -dotprod
    dotprod[dotprod < 0] <- 0
    dotprod[dotprod > 1] <- 1
    
    cols_fun <- colorRamp(c('royalblue', 'lightblue'))
    cols <- cols_fun(dotprod)  
    cols <- rgb(cols[,1], cols[,2], cols[,3], alpha = 255, maxColorValue = 255)
  }
  
  
  ntri <- ncol(obj$it)
  vertices <- with(obj, vb[, obj$it])
  vertices <- (vertices * 1000) + off
  
  # cols <- viridisLite::inferno(ntri, begin = 0.1, end = 0.3) |> sample() # colors for triangles
  # cols <- 'royalblue'
  pvertices <- vertices
  pvertices[] <- vertices
  
  nr <- nr_new(w, h, fill = 'blue')
  tigr_update(window, nr)

  frame <- 1
  nframes <- 600
  start <- Sys.time()
  for (frame in seq_len(nframes)) {

    nr_fill(nr, 'black')
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
    
    
    # if (frame == 10) break;
  }
  print(Sys.time() - start)
 
  tigr_close(window)
   
}



if (FALSE) {
  
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Investigate the 'double-draw' bug when rastering two triangles 
  # with a shared edge
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  library(grid)
  library(insitu)
  
  w   <- 40
  h   <- 40
  fac <- 5
  
  vertices <- matrix(c(
    10, 10,
    100, 100,
    100, 10, 
    
    10, 10, 
    10, 100, 
    100, 100,
    
    10, 100,
    50, 120,
    100, 100,
    
    100, 100,
    150, 50,
    100, 10
  ), nrow = 2) / 5
  
  cols <- scales::alpha(c('red', 'blue', 'green', 'yellow'), 0.2)
  
  nr <- nr_new(w, h)
  nr_tri_coords(nr, vertices, cols)
  grid.raster(nr, interpolate = FALSE)
  
}



if (FALSE) {
  
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Investigate the 'double-draw' bug when rastering two triangles 
  # with a shared edge
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  library(grid)
  library(insitu)
  library(naratigr)
  
  w   <- 800
  h   <- 800

  N  <- 100000
  xs <- runif(N, 1, w)
  ys <- runif(N, 1, h)
  
  off <- c(-(3:9), 3:9)
  dx1 <- sample(off, N, T)
  dx2 <- sample(off, N, T)
  
  dy1 <- sample(off, N, T)
  dy2 <- sample(off, N, T)
  
  xs <- as.vector(rbind(xs, xs + dx1, xs + dx2))
  ys <- as.vector(rbind(ys, ys + dy1, ys + dy2))
  
  vertices <- rbind(xs, ys)
  
  
  
  window <- tigr_open(w, h, expand = 2)
  
  nr <- nr_new(w, h, 'black')
  tigr_update(window, nr)
  
  cols <- viridis::magma(N)
  # cols <- scales::alpha(cols, 0.3)
  
  # start <- Sys.time()
  # for (frame in seq_len(300)) {
  bench::mark(
    nr_tri_coords(nr, vertices, cols, tris = 'ccw')
  )
    tigr_update(window, nr)
    
    # vertices <- ((vertices - w/2) * 1.005) + w/2
    
  # }
  # print(Sys.time() - start)
  
  
  
  tigr_close(window)
}














