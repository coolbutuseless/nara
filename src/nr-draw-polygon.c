
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sort an integer vector
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int scanline_sort_x(const void *a, const void *b) {
  return *(int *)a - *(int *)b;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw polygon [C interface]
// public domain code from
// https://www.alienryderflex.com/polygon_fill/
// not as efficient as something with an active edge table but it
// get me 30fps in "Another World" so I'm moving on.  Patches/PR welcomed!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nr_polygon(uint32_t *nr, int nr_width, int nr_height, int *x, int *y, int npoints, uint32_t color) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Pairs of values in 'nodeX' will have points drawn between them on 
  // a scanline.
  // 2024-12-30 Over-allocate here by a factor of 2.  Horizontal lines will cause
  //            2 points to be added to the 'nodeX' structure.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int *nodeX = (int *)malloc(2 * (size_t)npoints * sizeof(int));
  if (nodeX == NULL) {
    Rf_error("fill_polygon_c(): memory allocation failed for 'nodeX'");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Find vertical extents of all the polygons
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int ymin = INT_MAX;
  int ymax = INT_MIN;
  for (int i = 0; i < npoints; i++) {
    if (y[i] < ymin) {
      ymin = y[i];
    }
    if (y[i] > ymax) {
      ymax = y[i];
    }
  }
  
  ymin--;
  ymax++;
  
  if (ymin < 0) ymin = 0;
  if (ymax >= nr_height) ymax = nr_height - 1;
  
  // Rprintf("ymin/ymax : %i  %i\n", ymin, ymax);
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Precalc the gradient
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double *m = (double *)malloc((size_t)npoints * sizeof(double));
  if (m == NULL) {
    Rf_error("fill_polygon_c(): memory allocation failed for 'm'");
  }
  
  {
    int j = npoints - 1; // last point
    for (int i=0; i < npoints; i++) {
      m[i] = (double)(x[j] - x[i]) / (double)(y[j] - y[i]); 
      j = i; 
    }
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Loop through all the scanlines in the image which contain polygons
  // This would be faster if this used an Active Edge List
  // i.e. 
  //   - Sort lines by starting y, 
  //   - ignore lines when scanline < ystart
  //   - when scanline = ystart add to list of active edges
  //   - loop to find possible lines which intersect scanline runs faster
  //   - when scanline > yend, remove line from active edge list
  //
  // i.e. an active edge list would limit the search space for matching lines
  //      to just those lines which actually cross the scanline, so the 
  //      for loop would no longer be going through all 'npoints' to find intersects
  //
  // Further: no need to calculate 'nodeX' at each scanline.
  //   nodeX at scanline + 1 = nodeX at scaneline + 'm'
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  for (int scanline = ymin; scanline <= ymax; scanline++) {
    
    //  Build a list of nodes whose line crosses the scanline
    int nodes = 0; 
    int j = npoints - 1; // last point
    for (int i=0; i < npoints; i++) {
      if (((y[i] < scanline) && (y[j] >= scanline)) ||  
          ((y[j] < scanline) && (y[i] >= scanline))) {
        nodeX[nodes++] = (int) (x[i] + (scanline - y[i]) * m[i]); ///(double)(y[j] - y[i]) * (x[j] - x[i])); 
      } else if (y[i] == scanline && y[j] == scanline) {
        // Horizontal lines
        // nr_hline(nr, nr_width, nr_height, x[i], x[j], scanline, color);
        // 2024-Dec Some handling for polygons with horizontal lines between points.
        // Note: this may cause overdraw issues when alpha < 1. Untested.
        // Really need a better polygon algo.
        nodeX[nodes++] = (int) (x[i]); 
        nodeX[nodes++] = (int) (x[j]); 
      }
      j = i; 
    }
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Sort the x-coordinates along the scanline
    // void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*))
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    qsort(nodeX, (size_t)nodes, sizeof(int), scanline_sort_x);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //  Fill the pixels between node pairs.
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (int i = 0; i < nodes; i += 2) {
      if (nodeX[i] >= nr_width) break;
      if (nodeX[i + 1] >  0 ) {
        if (nodeX[i] < 0) nodeX[i] = 0;
        if (nodeX[i + 1] >= nr_width) nodeX[i + 1] = nr_width - 1;
        nr_hline(nr, nr_width, nr_height, nodeX[i], nodeX[i+1], scanline, color);
      }
    }
  } // end for(scanline)
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Tidy
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  free(m);
  free(nodeX);
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// R Polygon [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_polygons_single_(SEXP nr_, SEXP x_, SEXP y_, SEXP fill_, SEXP color_, SEXP thickness_, SEXP mitre_limit_) {
  
  assert_nativeraster(nr_);
  
  if (Rf_length(x_) != Rf_length(y_)) {
    Rf_error("Arguments 'x' and 'y' must be same length.");
  }
  
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  
  int nr_height = Rf_nrows(nr_);
  int nr_width  = Rf_ncols(nr_);
  
  uint32_t color = single_rcolor_to_int(color_);
  uint32_t fill   = single_rcolor_to_int(fill_);
  
  // get an int* from a numeric from R
  bool freex = false, freey = false;
  int N = calc_max_length(2, x_, y_);
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);
  
  // Rprintf("Polygon Fill: %i\n", fill);
  nr_polygon(nr, nr_width, nr_height, x, y, Rf_length(x_), fill);
  
  // Polygon outline only => Closed Polyline
  if (!is_transparent(color)) {
    double thickness = Rf_asReal(thickness_);
    double mitre_limit = Rf_asReal(mitre_limit_);
    bool close = true;
    nr_polyline(nr, nr_width, nr_height, x, y, N, color, thickness, mitre_limit, close);
  }
  
  // free and return
  if (freex) free(x);
  if (freey) free(y);
  return nr_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// R Polygon [R interface]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP nr_polygons_multi_(SEXP nr_, SEXP x_, SEXP y_, SEXP id_, SEXP fill_, SEXP color_, SEXP thickness_, SEXP mitre_limit_) {
  
  // Can we just do single polygon handling?
  if (Rf_isNull(id_)) {
    // Rprintf("Calling single\n");
    return nr_polygons_single_(nr_, x_, y_, fill_, color_, thickness_, mitre_limit_);
  }
  // Rprintf("Processing multiple\n");
  
  // Unpack native raster
  assert_nativeraster(nr_);
  uint32_t *nr = (uint32_t *)INTEGER(nr_);
  int nr_height = Rf_nrows(nr_);
  int nr_width  = Rf_ncols(nr_);
  
  // Sanity check
  int N = Rf_length(x_);
  if (Rf_length(y_) != N) {
    Rf_error("Arguments 'x' and 'y' must be same length.");
  }
  
  if (Rf_length(id_) != N) {
    Rf_error("Must supply 'id' arguments for all coordinates");
  }
  
  // Ensure 'id' is integer type for easier processing
  bool freeid = false;
  int *id = as_int32_vec(id_, N, &freeid);
  
  // How many polygons are defined by 'id'?
  int npolys = 1;
  int last_id = id[0];
  for (int i = 2; i < N; i++) {
    if (id[i] != last_id) {
      last_id = id[i];
      npolys++;
    }
  }
  
  // Rprintf("Number of polygons defined: %i\n", npolys);
  
  // get an int* from a numeric from R
  bool freex = false, freey = false;
  int *x = as_int32_vec(x_, N, &freex);
  int *y = as_int32_vec(y_, N, &freey);
  
  // Colors
  bool freecol = false, freefill = false;
  uint32_t *color = multi_rcolors_to_ints(color_, npolys, &freecol);
  uint32_t *fill  = multi_rcolors_to_ints(fill_ , npolys, &freefill);
  
  
  double thickness = Rf_asReal(thickness_);
  double mitre_limit = Rf_asReal(mitre_limit_);
  bool close = true;
  
  int poly_id = id[0];
  int poly_start = 0;
  int poly_end = 1;
  for (int i = 0; i < npolys - 1; i++) {
    
    while ( id[poly_end] == poly_id ) {
      poly_end++;
    } 
    
    int len = poly_end - poly_start;
    
    // Rprintf("Fill [%i] = %i\n", i, fill[i]);
    nr_polygon (nr, nr_width, nr_height, x + poly_start, y + poly_start, len, fill [i]);
    nr_polyline(nr, nr_width, nr_height, x + poly_start, y + poly_start, len, color[i], thickness, mitre_limit, close);
    
    poly_start = poly_end;
    poly_id    = id[poly_start];
    poly_end++;
  }
  
  
  // Final polygon
  int len = N - poly_start;
  // Rprintf("Final len: %i\n", len);
  nr_polygon (nr, nr_width, nr_height, x + poly_start, y + poly_start, len, fill [npolys - 1]);
  nr_polyline(nr, nr_width, nr_height, x + poly_start, y + poly_start, len, color[npolys - 1], 
              thickness, mitre_limit, close);
  
  
  
  
  // free and return
  if (freeid)   free(id);
  if (freex)    free(x);
  if (freey)    free(y);
  if (freecol)  free(color);
  if (freefill) free(fill);
  return nr_;
}

