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

/*
 JvPolyline is authored by Julien Vernay, in year 2025 (dev AT jvernay DOT fr).
 The software is released to the public domain, as explained below (UNLICENSE).
 If you find the software useful, you can share its accompanying blog post:
 https://jvernay.fr/en/blog/polyline-triangulation/
 
 This is free and unencumbered software released into the public domain.
 
 Anyone is free to copy, modify, publish, use, compile, sell, or
 distribute this software, either in source code form or as a compiled
 binary, for any purpose, commercial or non-commercial, and by any
 means.
 
 In jurisdictions that recognize copyright laws, the author or authors
 of this software dedicate any and all copyright interest in the
 software to the public domain. We make this dedication for the benefit
 of the public at large and to the detriment of our heirs and
 successors. We intend this dedication to be an overt act of
 relinquishment in perpetuity of all present and future rights to this
 software under copyright law.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
 
 For more information, please refer to <https://unlicense.org>
 */

#include "nr-draw-polyline-thick.h"
#include <math.h>

/// @param polyline[in] Input array of coordinates (X,Y) composing the polyline.
/// @param polylineCount Number of points in @a polyline .
/// @param linewidth Distance between the two parallel lines.
/// @param miterLimit Threshold to make angles sharp. Use `10.f` as default, `0.0f` to disable.
/// @param triangles[out] Output array of coordinates (X,Y), to be assembled three-by-three into
/// triangles.
/// @param triangleCapacity Number of triangles writable in @a triangles .
/// @return Number of triangles corresponding to the given @a polyline,
/// which can be bigger than @a triangleCapacity .
///
/// @attention
/// - @a pPolyline must have `2 * polylineCount` readable coordinates.
/// - The generated triangles count is at most `4 * (polylineCount - 2) + 2`.
///   Thus, there are at most `24 * (polylineCount - 2) + 6` coordinates written into @a triangles .
int32_t jvPolylineTriangulate(double const polyline[], int32_t polylineCount, double linewidth,
                              double miterLimit, double triangles[], int32_t triangleCapacity) {
  if (polylineCount <= 1)
    return 0; // Degenerate case: cannot trace a line from 0 or 1 point.
  if (miterLimit <= 0)
    miterLimit = 0;
  
  // Only compute these constants once.
  double halfThickness = linewidth / 2;
  double sqMiterThreshold = (linewidth * linewidth) * (miterLimit * miterLimit) / 4;
  
  // Iteration logic in the input and output arrays.
  // Indices represent coordinates (2 per point, 6 per triangle).
  int32_t idxIn = 0;
  int32_t idxInEnd = 2 * polylineCount;
  int32_t idxOut = 0;
  int32_t idxOutEnd = triangleCapacity * 6;
  
  // First point.
  double xA = polyline[idxIn];
  double yA = polyline[idxIn + 1];
  idxIn += 2;
  
  double xB, yB, lenAB = 0;
  while (idxIn < idxInEnd && lenAB == 0) {
    // Get second point, such that A and B are distinct.
    xB = polyline[idxIn];
    yB = polyline[idxIn + 1];
    lenAB = hypot(xB - xA, yB - yA);
    idxIn += 2;
  }
  if (lenAB == 0)
    return 0; // Degenerate case: all points are identical.
  
  for (; idxIn <= idxInEnd; idxIn += 2) {
    
    // Get next point C such that B and C are distinct.
    double xC, yC, lenBC;
    if (idxIn < idxInEnd) {
      xC = polyline[idxIn];
      yC = polyline[idxIn + 1];
      lenBC = hypot(xC - xB, yC - yB);
    }
    else {
      // [AB] is the last segment to render, no join is needed at the end.
      // Making C = A such that (AB) and (BC) are aligned, thus no join will be generated.
      xC = xA;
      yC = yA;
      lenBC = lenAB;
    }
    if (lenBC == 0)
      continue; // Empty segment, do nothing.
    
    // Compute quad for segment AB.
    
    double xAB = xB - xA, yAB = yB - yA;
    double xAA1 = halfThickness / lenAB * -yAB;
    double yAA1 = halfThickness / lenAB * xAB;
    double xA1 = xA + xAA1, yA1 = yA + yAA1;
    double xA2 = xA - xAA1, yA2 = yA - yAA1;
    double xA1p = xB + xAA1, yA1p = yB + yAA1;
    double xA2p = xB - xAA1, yA2p = yB - yAA1;
    
    // Encode quad for segment AB as two triangles.
    if (idxOut < idxOutEnd) {
      triangles[idxOut + 0] = xA1;
      triangles[idxOut + 1] = yA1;
      triangles[idxOut + 2] = xA2;
      triangles[idxOut + 3] = yA2;
      triangles[idxOut + 4] = xA1p;
      triangles[idxOut + 5] = yA1p;
    }
    idxOut += 6;
    if (idxOut < idxOutEnd) {
      triangles[idxOut + 0] = xA2;
      triangles[idxOut + 1] = yA2;
      triangles[idxOut + 2] = xA1p;
      triangles[idxOut + 3] = yA1p;
      triangles[idxOut + 4] = xA2p;
      triangles[idxOut + 5] = yA2p;
    }
    idxOut += 6;
    
    // Determine the ABC angle's orientation.
    
    double xBC = xC - xB, yBC = yC - yB;
    double zAB_BC = xAB * yBC - yAB * xBC;
    
    // If zAB_BC == 0, A B C are aligned and no join is needed.
    if (zAB_BC != 0) {
      // Compute the endpoints of the next segment.
      
      double xBB1 = halfThickness / lenBC * -yBC;
      double yBB1 = halfThickness / lenBC * xBC;
      double xB1 = xB + xBB1, yB1 = yB + yBB1;
      double xB2 = xB - xBB1, yB2 = yB - yBB1;
      
      // Generate Bevel join triangle.
      
      if (idxOut < idxOutEnd) {
        if (zAB_BC < 0) {
          triangles[idxOut + 0] = xB;
          triangles[idxOut + 1] = yB;
          triangles[idxOut + 2] = xA1p;
          triangles[idxOut + 3] = yA1p;
          triangles[idxOut + 4] = xB1;
          triangles[idxOut + 5] = yB1;
        }
        else {
          triangles[idxOut + 0] = xB;
          triangles[idxOut + 1] = yB;
          triangles[idxOut + 2] = xA2p;
          triangles[idxOut + 3] = yA2p;
          triangles[idxOut + 4] = xB2;
          triangles[idxOut + 5] = yB2;
        }
      }
      idxOut += 6;
      
      // Generate Miter triangle.
      
      if (miterLimit > 0) {
        double xM, yM;
        if (zAB_BC < 0) {
          // Compute M, the intersection between (A1;AB) and (B1;BC)
          double alpha = (yBC * (xB1 - xA1) + xBC * (yA1 - yB1)) / zAB_BC;
          xM = xA1 + alpha * xAB;
          yM = yA1 + alpha * yAB;
        }
        else {
          // Compute M, the intersection between (A2;AB) and (B2;BC)
          double alpha = (yBC * (xB2 - xA2) + xBC * (yA2 - yB2)) / zAB_BC;
          xM = xA2 + alpha * xAB;
          yM = yA2 + alpha * yAB;
        }
        
        double sqLenBM = (xM - xB) * (xM - xB) + (yM - yB) * (yM - yB);
        if (sqLenBM <= sqMiterThreshold) {
          if (idxOut < idxOutEnd) {
            if (zAB_BC < 0) {
              triangles[idxOut + 0] = xM;
              triangles[idxOut + 1] = yM;
              triangles[idxOut + 2] = xA1p;
              triangles[idxOut + 3] = yA1p;
              triangles[idxOut + 4] = xB1;
              triangles[idxOut + 5] = yB1;
            }
            else {
              triangles[idxOut + 0] = xM;
              triangles[idxOut + 1] = yM;
              triangles[idxOut + 2] = xA2p;
              triangles[idxOut + 3] = yA2p;
              triangles[idxOut + 4] = xB2;
              triangles[idxOut + 5] = yB2;
            }
          }
          idxOut += 6;
        }
      }
    }
    // Prepare for next segment.
    xA = xB;
    yA = yB;
    xB = xC;
    yB = yC;
    lenAB = lenBC;
  }
  
  // Returns number of triangle
  return idxOut / 6;
}



void nr_polyline_thick(uint32_t *nr, int nr_width, int nr_height, int *x, int *y,
                       int npoints, uint32_t color, double linewidth, double mitre_limit, bool close) {
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // How many polylines are there?
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int polylineCount = npoints;
  if (close) {
    polylineCount += 2;  // close the polygon by manually adding the first two points at the end
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Allocation room for the triangles
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int triangleCapacity = 24 * (polylineCount - 2) + 6;
  double *triangles = malloc((size_t)triangleCapacity * sizeof(double) * 2); // Safety factor = 2x
  if (triangles == NULL) {
    Rf_error("nr_polyline_thick_(): Couldn't allocate 'triangles'");
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Convert the triangles from integers to 
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double *polyline = malloc(2 * (size_t)polylineCount * sizeof(double)); 
  if (polyline == NULL) {
    Rf_error("nr_polyline_thick_(): Couldn't allocate 'polyline'");
  }
  for (int i = 0; i < npoints; ++i) {
    polyline[2 * i + 0] = x[i];
    polyline[2 * i + 1] = y[i];
  }
  
  // Add extra point if close=TRUE
  if (close) {
    polyline[2 * (npoints) + 0] = x[0];
    polyline[2 * (npoints) + 1] = y[0];
    polyline[2 * (npoints) + 2] = x[1];
    polyline[2 * (npoints) + 3] = y[1];
  }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Call core algo to create triangles to represent the polyline
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int ntris = jvPolylineTriangulate(polyline, polylineCount, linewidth, 
                                    mitre_limit, triangles, triangleCapacity);
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Draw triangles using nr_polygon()
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int *xs = malloc((size_t)ntris * 3 * sizeof(int));
  int *ys = malloc((size_t)ntris * 3 * sizeof(int));
  if (xs == NULL || ys == NULL) {
    Rf_error("nr_polyline_thick_(): Couldn't allocate triangle coords 'xs', 'ys'");
  }
  for (int i = 0; i < ntris * 3; ++i) {
    xs[i] = (int)round(triangles[2 * i + 0]);
    ys[i] = (int)round(triangles[2 * i + 1]);
  }
  
#define TRANSPARENT_BLACK 0x00000000u
  
  draw_mode_t draw_mode = RESPECT_ALPHA;
  
  double thickness_tri = 1; // ignored
  double mitre_limit_tri = 1; // ignored
  for (int i = 0; i < ntris; ++i) {
    nr_polygon(nr, nr_width, nr_height, xs + 3 * i, ys + 3 * i, 3, color, TRANSPARENT_BLACK, thickness_tri, mitre_limit_tri, draw_mode);
  }
  
  
  // free and return
  free(ys);
  free(xs);
  free(triangles);
  free(polyline);
  
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw polyline [R interace]
// 
// @param nr native raster (modified in-place)
// @param nr_width,nr_height dimensions
// @param x,y locations
// @param color colour
// @param linewidth line linewidth
// @param close should the polyline be closed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SEXP nr_polyline_thick_(SEXP nr_, SEXP x_, SEXP y_, SEXP color_, SEXP linewidth_, 
//                         SEXP mitre_limit_, SEXP close_) {
//   
//   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   // Unpack args
//   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   assert_nativeraster(nr_);
//   uint32_t *nr = (uint32_t *)INTEGER(nr_);
//   int nr_height = Rf_nrows(nr_);
//   int nr_width  = Rf_ncols(nr_);
//   uint32_t color = single_rcolor_to_int(color_);
//   
//   if (Rf_length(x_) != Rf_length(y_)) {
//     Rf_error("Arguments 'x' and 'y' must be same length.");
//   }
//   
//   double mitre_limit = Rf_asReal(mitre_limit_);
//   double linewidth   = Rf_asReal(linewidth_);
//   
//   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   // Ensure we have an integer vector for x and y
//   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   bool freex = false, freey = false;
//   int N = calc_max_length(2, x_, y_);
//   int *x = as_int32_vec(x_, N, &freex);
//   int *y = as_int32_vec(y_, N, &freey);
//   bool close = Rf_asLogical(close_);
//   
//   nr_polyline_thick(nr, nr_width, nr_height, x, y,
//                          N, color, linewidth, mitre_limit, close);
// 
//   if (freex) free(x);
//   if (freey) free(y);
//   return nr_;
// }

