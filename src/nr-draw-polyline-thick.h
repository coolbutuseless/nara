/// @file jv_polyline.h
/// @author Julien Vernay, 2025
/// @copyright UNLICENSE ( https://unlicense.org/ )
/// @brief An unobtrusive and freestanding implementation of triangulation of polylines.
/// See the accompanying blog post:
/// https://jvernay.fr/en/blog/polyline-triangulation/

#pragma once

#include <stdbool.h>
#include <stdint.h>

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
// int32_t jvPolylineTriangulate(double const polyline[], int32_t polylineCount, double linewidth,
//                               double miterLimit, double triangles[], int32_t triangleCapacity);

void nr_polyline_thick(uint32_t *nr, int nr_width, int nr_height, int *x, int *y,
                       int npoints, uint32_t color, double linewidth, double mitre_limit, bool close, draw_mode_t draw_mode);
