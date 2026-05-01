
#define R_NO_REMAP

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "color.h"
#include "nr-core.h"
#include "nr-utils.h"
#include "nr-draw.h"

#define FMT_WIDE 1
#define FMT_LONG 2

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// is (cx, cy) to the left of the line from (ax, ay) to (bx, by)?
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// int left_of(int ax, int ay, int bx, int by, int cx, int cy) {
//   return ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay)) >= 0;
// }

double det_dbl(double ax, double ay, double bx, double by, double cx, double cy) {
  return ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
}

int det(int ax, int ay, int bx, int by, int cx, int cy) {
  return ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
}


int min3(int a, int b, int c) {
  int temp = (a < b)    ? a : b;
  return (c < temp) ? c : temp;
}


int max3(int a, int b, int c) {
  int temp = (a > b) ? a : b;
  return (c > temp) ? c : temp;
}


bool is_left_top_edge(int x0, int y0, int x1, int y1) {
  bool is_left_edge = y1 > y0;
  bool is_top_edge  = (y1 == y0) && (x1 < x0);
  return is_left_edge || is_top_edge;
}


#define TRI_CCW 1
#define TRI_CW  2
#define TRI_ALL 3


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void draw_tri(uint32_t *nr, int nr_width, int nr_height, 
              double x0d, double y0d, 
              double x1d, double y1d, 
              double x2d, double y2d,
              int color, 
              draw_mode_t draw_mode,
              int tri_mode
              ) {
  
  // Calculate the determinant using the full resolution 'double' data.
  // If we round to integer to calculate the overall triangle area, then
  // small triangles are going to have all 3 vertices collapse to a single
  // coordinate.  The det(a, b, c) where a,b,c are all the same point 
  // evaluates to zero.  So every small triangle with all vertices the same
  // will be "facing" the camera and rendered. This leads to freckles/noise
  // in the output.
  bool oriented_ccw = det_dbl(x0d, y0d, x1d, y1d, x2d, y2d) >= 0;
  
  if (tri_mode == TRI_CCW && !oriented_ccw) return;
  if (tri_mode == TRI_CW  &&  oriented_ccw) return;
  
  if (!oriented_ccw) {
    // swap 2 verts to make it ok
    double tmp;
    tmp = x0d; x0d = x1d; x1d = tmp;
    tmp = y0d; y0d = y1d; y1d = tmp;
  }
  
  int x0 = (int)round(x0d);
  int y0 = (int)round(y0d);
  int x1 = (int)round(x1d);
  int y1 = (int)round(y1d);
  int x2 = (int)round(x2d);
  int y2 = (int)round(y2d);
  
  
  // Calculate bounding box of the triangle
  int xmin = min3(x0, x1, x2);
  int xmax = max3(x0, x1, x2);
  int ymin = min3(y0, y1, y2);
  int ymax = max3(y0, y1, y2);
  
  // If bounding box is off the edge of the canvas, clamp it to the canvas limits
  xmin = xmin < 0 ? 0 : xmin;
  xmax = xmax >= nr_width ? nr_width - 1 : xmax;
  ymin = ymin < 0 ? 0 : ymin;
  ymax = ymax >= nr_height ? nr_height - 1 : ymax;
  
  
  // Only need to calculate the determinant once, 
  // and then the determinant at every other position is just a 
  // linear offset.
  // So the det at each point can be done with just 3 additions, 
  // rather than 3 calls to det()
  
  // Find determinants at top left of bbox
  int w00 = det(x0, y0, x1, y1, xmin, ymin);
  int w10 = det(x1, y1, x2, y2, xmin, ymin);
  int w20 = det(x2, y2, x0, y0, xmin, ymin);

  // Find how much the determinant changes for a 1-pixel
  // change in cx and cy by doing some manual 
  // maths on the det() function 
  int dx0 = y1 - y0; int dy0 = x0 - x1;
  int dx1 = y2 - y1; int dy1 = x1 - x2;
  int dx2 = y0 - y2; int dy2 = x2 - x0;
  
  
  // Correct for edge overlap
  w00 += (is_left_top_edge(x0, y0, x1, y1)) ? -1 : 0;
  w10 += (is_left_top_edge(x1, y1, x2, y2)) ? -1 : 0;
  w20 += (is_left_top_edge(x2, y2, x0, y0)) ? -1 : 0;
  
  
  // Loop over the bounding box
  //   if all three determinants are >= 0, the point is interior to triangle
  for (int y = ymin; y <= ymax; y++) {
  
    // Initialise determinants at start of this row  
    int w0 = w00;
    int w1 = w10;
    int w2 = w20;
    
    for (int x = xmin; x <= xmax; x++) {
      
      // This is what we'd do if we wanted to fully calculate the
      // determinant at every pixel location
      // int w0 = det(x0, y0, x1, y1, x, y);
      // int w1 = det(x1, y1, x2, y2, x, y);
      // int w2 = det(x2, y2, x0, y0, x, y);
      
      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        nr_point(nr, nr_width, nr_height, x, y, color, draw_mode);
      }
      
      // Update determinants for next pixel in this row
      w0 += dx0;
      w1 += dx1;
      w2 += dx2;
    } // end of row
    
    // Update determinants for next row
    w00 += dy0;
    w10 += dy1;
    w20 += dy2;
  }
  
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Coordinate matrix input
//    - rows represnet x,y,... coordinates
//    - only first 2 coordinates are used.  Others are skipped
//    - Each triangle defined by a sequence of three columns giving the 
//      three coordinates for the vertices
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_tri_coords_(SEXP nr_, SEXP coords_, SEXP color_, SEXP tri_mode_, 
                    SEXP draw_mode_) {
  
  // Sanity Check
  assert_nativeraster(nr_);
  
  // Which triangles are we rendering?
  const char *tri_mode_str = CHAR(STRING_ELT(tri_mode_, 0));
  int tri_mode = TRI_CCW;
  if (strcmp(tri_mode_str, "ccw") == 0) {
    tri_mode = TRI_CCW;
  } else if (strcmp(tri_mode_str, "cw") == 0) {
    tri_mode = TRI_CW;
  } else if (strcmp(tri_mode_str, "all") == 0) {
    tri_mode = TRI_ALL;
  } else {
    Rf_error("nr_tri_coords_(): tri_mode not understood: '%s'", tri_mode_str);
  }
  
  // Unpack the native raster
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  int nr_height = Rf_nrows(nr_);
  int nr_width  = Rf_ncols(nr_);
  
  // Sanity check coords
  if (!Rf_isMatrix(coords_)) {
    Rf_error("nr_tri_coords_(): 'coords' data must a matrix");
  }
  
  // Infer the number of coordinates and the stride
  // The stride gives an indication of how far apart the coordiantes are.
  // This is because a user might give:
  //  * matrix with 2 rows x,y
  //  * matrix with 3 rows x,y,z
  int n_coords = Rf_ncols(coords_);
  int n_dims   = Rf_nrows(coords_);
  
  // Unpack the coordinates data as 'double' values
  bool free_coords = false;
  double *coords = as_double_vec(coords_, Rf_length(coords_), &free_coords);
  
  if (n_coords % 3 != 0) {
    Rf_error("nr_tri_coords_(): ncols(coords) is not a multiple of 3: %i", n_coords);
  }
  int n_tris = n_coords / 3;

  // Unpack a color per triangle
  bool free_color = false;
  uint32_t *color = multi_rcolors_to_ints(color_, n_tris, &free_color);
  
  draw_mode_t draw_mode = Rf_asInteger(draw_mode_);
  
  double *pc = coords;
  
  for (int i = 0; i < n_tris; i++) {
    
    double *pc0 = pc + 0 * n_dims; // 1st column of coordiantes
    double *pc1 = pc + 1 * n_dims; // 2nd column of coordiantes
    double *pc2 = pc + 2 * n_dims; // 3rd column of coordiantes
    
    draw_tri(
      nr, nr_width, nr_height, 
      pc0[0], pc0[1], // first + second element in column => (x, y) coords
      pc1[0], pc1[1],
      pc2[0], pc2[1],
      color[i], draw_mode, tri_mode
    ); 
    
    // Skip ahead to the next triplet of vertex coordinates
    pc += 3 * n_dims;
  }
  
  
  if (free_coords) free(coords);
  if (free_color)  free(color);
  return nr_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Coordinate matrix input
//    - rows represnet x,y,... coordinates
//    - only first 2 coordinates are used.  Others are skipped
//    - Each triangle defined by a sequence of three columns giving the 
//      three coordinates for the vertices
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_tri_mesh_(SEXP nr_, SEXP vertices_, SEXP indices_, SEXP color_, 
                  SEXP tri_mode_, SEXP draw_mode_) {
  
  // Sanity Check
  assert_nativeraster(nr_);
  
  // Which triangles are we rendering?
  const char *tri_mode_str = CHAR(STRING_ELT(tri_mode_, 0));
  int tri_mode = TRI_CCW;
  if (strcmp(tri_mode_str, "ccw") == 0) {
    tri_mode = TRI_CCW;
  } else if (strcmp(tri_mode_str, "cw") == 0) {
    tri_mode = TRI_CW;
  } else if (strcmp(tri_mode_str, "all") == 0) {
    tri_mode = TRI_ALL;
  } else {
    Rf_error("nr_tri_mesh_(): tri_mode not understood: '%s'", tri_mode_str);
  }
  
  // Unpack the native raster
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  int nr_height = Rf_nrows(nr_);
  int nr_width  = Rf_ncols(nr_);
  
  // Sanity check coords
  if (!Rf_isMatrix(vertices_)) {
    Rf_error("nr_tri_mesh_(): 'vertices' data must a matrix");
  }
  if (!Rf_isMatrix(indices_)) {
    Rf_error("nr_tri_mesh_(): 'indices' data must a matrix");
  }
  if (Rf_nrows(indices_) != 3) {
    Rf_error("nr_tri_mesh_(): 'indices' must have exactly 3 rows");
  }
  
  // Infer the number of coordinates and the stride
  // The stride gives an indication of how far apart the coordiantes are.
  // This is because a user might give:
  //  * matrix with 2 rows x,y
  //  * matrix with 3 rows x,y,z
  int n_tris  = Rf_ncols(indices_);
  int n_verts = Rf_ncols(vertices_);
  int n_dims  = Rf_nrows(vertices_);
  
  // Unpack the vertex data as 'double' values
  bool free_vertices = false;
  double *vertices = as_double_vec(vertices_, Rf_length(vertices_), &free_vertices);

  // Unpack the index data as integers
  bool free_indices = false;
  int *indices = as_int32_vec(indices_, Rf_length(indices_), &free_indices);


  // Unpack a color per triangle
  bool free_color = false;
  uint32_t *color = multi_rcolors_to_ints(color_, n_tris, &free_color);

  draw_mode_t draw_mode = Rf_asInteger(draw_mode_);

  Rprintf("tris: %i  indices: %i  verts: %i\n", n_tris, Rf_length(indices_), n_verts);
  
  int *ip = indices;
  for (int i = 0; i < n_tris; i++) {

    int v0 = ip[0] - 1; // Convert from R 1-indexed to C 0-indexed
    int v1 = ip[1] - 1; // Convert from R 1-indexed to C 0-indexed
    int v2 = ip[2] - 1; // Convert from R 1-indexed to C 0-indexed

    draw_tri(
      nr, nr_width, nr_height,
      //                      x,                         y
      vertices[v0 * n_dims + 0], vertices[v0 * n_dims + 1],
      vertices[v1 * n_dims + 0], vertices[v1 * n_dims + 1],
      vertices[v2 * n_dims + 0], vertices[v2 * n_dims + 1],
      color[i], draw_mode, tri_mode
    );
    
    ip += 3;
  }


  if (free_vertices) free(vertices);
  if (free_indices)  free(indices);
  if (free_color)    free(color);
  return nr_;
}












