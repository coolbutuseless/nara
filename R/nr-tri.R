
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw multiple triangles from mesh data
#' 
#' Mesh data is a similar format to the \code{mesh3d} class of data 
#' defined in the \code{rgl} package.
#' 
#' @inheritParams nr_fill
#' @inheritParams nr_rect
#' @param vertices Wide matrix of vertex coordinates where the first two rows are
#'        x and y coordinates.  Extra coordinates are ignored.
#' @param indices 3xN wide integer matrix of index information. Each column 
#'        holds the 3 indices which indicate the column index of the triangle vertices defined in the
#'        \code{vertices} argument.
#' @param coords Wide numeric matrix of direct coordinate data for each triangle where
#'        the first two rows are x and y coordinates (extra rows are ignored)
#'        Each group of 3 columns defines the (x,y) coordinates for one
#'        triangle.
#' @param color color specification. Single color or one color per tri
#' @param tris Which triangles should be drawn?  Valid options: 'all' (default), 
#'        'ccw', 'cw'. The options 'ccw' and 'cw' limit plotting to those triangles
#'        where the order of the vertices are defined in a counter-clockwise
#'        or clockwise manner respectively.  Note: when considering orientation
#'        or triangles remember that the y-axis is defined vertically \emph{down}
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
#' plot(nr)
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
#' plot(nr)
#' 
#' # Matrix of indices indicating which vertices make up each triangle
#' indices <- matrix(sample(length(xs), 3 * n_tri), nrow = 3) 
#' 
#' nr_tri_mesh(nr, vertices, indices, cols, tris = 'all')
#' plot(nr)
#' 
#' @return Invisibly return the supplied native raster image which was been modified in-place
#' @family drawing functions
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_tri_mesh <- function(nr, vertices, indices, color, tris = 'all', use_alpha = TRUE) {
  invisible(
    .Call(nr_tri_mesh_, nr, vertices, indices, color, tris, use_alpha)
  )
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_tri_mesh
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_tri_coords <- function(nr, coords, color, tris = 'all', use_alpha = TRUE) {
  invisible(
    .Call(nr_tri_coords_, nr, coords, color, tris, use_alpha)
  )
}



