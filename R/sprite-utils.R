

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
  
  xoff <-  20
  yoff <-   1
  
  while (TRUE) {
    nr <- nr_new(w, h, 'grey90')
    nr_blit(nr, xoff, yoff, ss)
    nr_rect(nr, x-1, y-1, w_sprite+2, h_sprite+2, fill = NA, colour = '#00000020')
    grid.newpage(); 
    
    grid.raster(nr, interpolate = FALSE)
    
    # Stamp it
    grid.rect(x = 0, y = 0, width = 0.1, height = 0.1, just = c(0, 0), gp = gpar(fill = '#0000ff80'), default.units = 'npc')
    
    # Move ss
    grid.rect(x = 0   , y = 0.45, width = 0.1, height = 0.1, just = c(0, 0), gp = gpar(fill = '#ff000080'), default.units = 'npc')
    grid.rect(x = 0.45, y = 0.0 , width = 0.1, height = 0.1, just = c(0, 0), gp = gpar(fill = '#ff000080'), default.units = 'npc')
    grid.rect(x = 0.45, y = 0.90, width = 0.1, height = 0.1, just = c(0, 0), gp = gpar(fill = '#ff000080'), default.units = 'npc')
    grid.rect(x = 0.90, y = 0.45, width = 0.1, height = 0.1, just = c(0, 0), gp = gpar(fill = '#ff000080'), default.units = 'npc')
    
    loc <- locator(1)  
    loc$x <- loc$x - 0.5
    loc$y <- loc$y - 0.5
    
    # print(unlist(loc))
    
    if (loc$x < -0.4 && loc$y < -0.4) {
      # Stamp it
      cat(sprintf("c(%i, %i, %i, %i)\n", x - xoff + 1, y - yoff + 1, w_sprite, h_sprite))
    } else if (loc$x <= -0.4 && loc$y >= -0.05 && loc$y <= 0.05) {
      xoff <- xoff + 20
    } else if (loc$x >= 0.4 && loc$y >= -0.05 && loc$y <= 0.05) {
      xoff <- xoff - 20
    } else if (loc$y <= -0.4 && loc$x >= -0.05 && loc$x <= 0.05) {
      yoff <- yoff + 20
    } else if (loc$y >= 0.4 && loc$x >= -0.05 && loc$x <= 0.05) {
      yoff <- yoff - 20
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


if (FALSE) {
  # nr_rect(nr, 1, 1, 30, 24)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(  7, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c( 40, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c( 74, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(107, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(141, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(174, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(204, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(238, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(270, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(301, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(333, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(365, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(398, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(429, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  nr <- nr_new(100, 100, 'grey90'); nr_blit2(nr, 1, 1, ss, c(461, 5, 27, 24)); grid.newpage(); grid.raster(nr, interpolate = FALSE)
  
}