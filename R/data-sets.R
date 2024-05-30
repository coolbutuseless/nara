
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
  
  x11(type = 'dbcairo', width = 7, height = 7)
  dev.control('inhibit')
  
  even <-  1 + (0:5) * 64
  odd  <- 33 + (0:5) * 64
  
  tile_idxs <- grep("basic|house|tree", names(isometric_landscape))
  probs <- rep(1, length(tile_idxs))
  probs[1] <- 30
  
  grid.newpage();
  nr <- nr_new(420, 420, 'white')
  
  start <- Sys.time()
  for (i in 1:300) {
    nr_fill(nr, 'white')
    for (y in seq(340, 0, -32)) {
      select <- sample(tile_idxs, length(even), T, prob = probs)
      for (i in seq_along(even)) {
        nr_blit(nr, even[i], y, isometric_landscape[[select[i]]])
      }
      
      select <- sample(tile_idxs, length(odd), T, prob = probs)
      for (i in seq_along(odd)) {
        nr_blit(nr,  odd[i], y - 16, isometric_landscape[[select[[i]]]])
      }
    }
    
    dev.hold()
    grid.raster(nr, interpolate = FALSE)
    dev.flush()
    # Sys.sleep(0.01)
  }
  Sys.time() - start
  
}
