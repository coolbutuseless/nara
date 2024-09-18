
# nara 0.1.1.9024 2024-08-18

* Remove 'isocubes()'
* Remove 'op' argument. 
* Standardise argument ordering
* Remove 'dino' sprites
* Add 'deer' spritesheet and isometric landscape tiles
* `str_cols_to_packed_cols()` for `transparent` and `NA_character` now returns value
  equivalent to `#ffffff00`
* Add
    * `nr_crop()`
    * `nr_flipv()`
    * `nr_fliph()`
    * `matrix_to_nr()`
* Refactored color conversion routines. 
* Fast conversions in C
    * `array_to_nr()`
    * `nr_to_array()`
    * `raster_to_nr()`
    * `nr_to_raster()`
    * `magick_to_nr()`, `nr_to_magick()`
* Optimized blitting operations in C
* Added `blit3()` and `blit4()` mega-blitters
* Fixed(?) off-by-one bug in blitter core
* Drop inspection of "channels" attribute all together.
* As of 0.1.1.9009 2024-06-08 coordinate system is now (0, 0) at top-left
* Fixed rendering of borders for rectangles when both colour and fill are 
  translucent.
* Rationalize 'blit' methods (still need C implementations)
* Move all 'magick' related functions into single R file.  Include gif and mp4 output.
* Use fast hashed lookup for R color names like 'red', 'blue', etc
* Handle logical NAs as transparent colour
* pass through more arguments for `nrs_to_gif()` and `nrs_to_mp4()`
* `hjust` and `vjust` added for 
    * `nr_blit()`
    * `nr_rect()`
* added `nr_blit_list()` and `nr_blit_grid()`.
* added `nr_resize_nn()` and `nr_resize_bilinear()`. `nr_scale()` is now a wrapper around `nr_resize_nn()`
* Fixed bug in polygon rendering
* Parse image sequences from `magick` into nativeRaster
* Export C functions for use in other packages via `LinkingTo`
    * draw points
* Added `nr_polygons()`

# nara 0.1.1 2022-07-30

* added `nr_isocube()` for drawing isometric cubes
* Use this version for 'Another World' demo

# nara 0.1.0

* Initial release
