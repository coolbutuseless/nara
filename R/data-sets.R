
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Sprite sheet for some animated characters
#' 
#' A sprite sheet for animated characters.  This spritesheet is in the
#' public domain.
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"king"

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Locations of 16 sprites within the \code{king} spritesheet
#' 
#' List of 16 numeric vectors. Each vector consistes of (x, y, width, height)
#' location of a sprite within the spritesheet.
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"king_locs"

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' A list of 36 isometric tiles in nativeRaster format
#' 
#' These tiles are in the public domain
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"isometric_landscape"


if (FALSE) {
  
  library(grid)
  
  even <-  1 + (0:5) * 60
  odd  <- 31 + (0:5) * 60
  
  tile_idxs <- grep("basic|house|tree", names(isometric_landscape))
  probs <- rep(1, length(tile_idxs))
  probs[1] <- 30
  
  grid.newpage();
  nr <- nr_new(420, 420, 'white')
  
  nr_fill(nr, 'white')
  for (y in seq(340, 0, -30)) {
    select <- sample(tile_idxs, length(even), T, prob = probs)
    for (i in seq_along(even)) {
      nr_blit(nr, even[i], y, isometric_landscape[[select[i]]])
    }
    
    select <- sample(tile_idxs, length(odd), T, prob = probs)
    for (i in seq_along(odd)) {
      nr_blit(nr,  odd[i], y - 15, isometric_landscape[[select[[i]]]])
    }
  }
  
  grid.raster(nr, interpolate = FALSE)
  
}


if (FALSE) {
  
  nr <- nr_new(130, 130)
  nr_blit(nr, 1 + 32, 21 + 16, isometric_landscape$basic1)
  nr_blit(nr, 1 + 64, 21     , isometric_landscape$basic1)
  nr_blit(nr, 1 +  2, 21 +  1, isometric_landscape$basic1)
  nr_blit(nr, 1 + 34, 21 - 15, isometric_landscape$basic1)
  grid.newpage(); grid.raster(nr, interpolate = FALSE)
  
}