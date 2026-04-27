
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Draw multiple triangles
#' 
#' @inheritParams nr_fill
#' @param vertices matrix of vertex information. Each column represents
#'        a single vertex.  Only the first 2 rows are used
#' @param idx matrix of index information with 3 rows.  Each column represents
#'        the indices of a single triangle
#' @param col color specification. Single color or one color per tri
#' @param cull culling of triangles
#' @param draw_all adjust all triangles to be the correct orientation
#' 
#' @return XYZ
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_tri <- function(vertices, idx, col, cull = c('cw', 'none', 'ccw'), draw_all = FALSE) {
  
}