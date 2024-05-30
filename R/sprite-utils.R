

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Helper for finding coordinates in a spritesheet
#' 
#' @param ss sprite sheet (nativeRaster)
#' 
#' @noRd
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
sprite_finder <- function(ss, w, h, w_sprite, h_sprite, x = 1, y = 1) {
  
  
  
}

if (FALSE) {
  
  
  source <- "working/dino-spritesheet.png"
  ss <- read_png(source, transparent = list(-1594494))
  if (FALSE) {
    grid.newpage(); grid.raster(ss)
  }
  
  x <- 7
  y <- 5
  
  w <- 100
  h <- 100
  w_sprite <- 27
  h_sprite <- 25
  
  plot.new()
  
  while (TRUE) {
    nr <- nr_new(w, h, 'grey90')
    nr_blit(nr, 1, 1, ss)
    nr_rect(nr, x-1, y-1, w_sprite+2, h_sprite+2, fill = NA, colour = '#00000020')
    grid.newpage(); 
    grid.rect(x = 0, y = 0, width = 0.1, height = 0.1, just = c(0, 0), gp = gpar(fill = 'blue'))
    grid.raster(nr, interpolate = FALSE)
    
    loc <- locator(1)  
    loc$x <- loc$x - 0.5
    loc$y <- loc$y - 0.5
    
    # print(unlist(loc))
    
    if (loc$x < -0.4 && loc$y < -0.4) {
      cat(sprintf("c(%i, %i, %i, %i)\n", x, y, w_sprite, h_sprite))
    } else if (loc$x > 0) {
      if (loc$x > abs(loc$y)) {
        x <- x + 1
      } else if (loc$y > 0) {
        y <- y + 1
      } else if (loc$y < 0) {
        y <- y - 1
      }
    } else {
      if (abs(loc$x) > abs(loc$y)) {
        x <- x - 1
      } else if (loc$y > 0) {
        y <- y + 1
      } else if (loc$y < 0) {
        y <- y - 1
      }
    }
  }
  
}
