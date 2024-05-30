

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
  
  w_sprite <- 30
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
      xoff <- xoff + 30
    } else if (loc$x >= 0.4 && loc$y >= -0.05 && loc$y <= 0.05) {
      xoff <- xoff - 30
    } else if (loc$y <= -0.4 && loc$x >= -0.05 && loc$x <= 0.05) {
      yoff <- yoff + 30
    } else if (loc$y >= 0.4 && loc$x >= -0.05 && loc$x <= 0.05) {
      yoff <- yoff - 30
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
  top_row <- list(
    c(  3, 65, 30, 25),
    c( 32, 65, 30, 25),
    c( 62, 65, 30, 25),
    c( 92, 65, 30, 25),
    c(118, 65, 30, 25),
    c(145, 65, 30, 25),
    c(171, 65, 30, 25),
    c(200, 65, 30, 25),
    c(229, 65, 30, 25),
    c(257, 65, 30, 25),
    c(286, 65, 30, 25),
    c(314, 65, 30, 25),
    c(342, 65, 30, 25),
    c(370, 65, 30, 25),
    c(398, 65, 30, 25),
    c(427, 65, 30, 25),
    c(460, 65, 30, 25),
    c(493, 65, 30, 25),
    c(526, 65, 30, 25),
    c(558, 65, 30, 25),
    c(588, 65, 30, 25),
    c(616, 65, 30, 25),
    c(644, 65, 30, 25),
    c(675, 65, 30, 25),
    c(710, 65, 30, 25),
    c(744, 65, 30, 25),
    c(779, 65, 30, 25),
    c(811, 66, 30, 25),
    c(839, 66, 30, 25),
    c(867, 66, 30, 25),
    c(896, 65, 30, 25),
    c(925, 65, 30, 25)
  )
  
  library(grid)
  
  x11(type = 'dbcairo', width = 7, height = 3)
  dev.control('inhibit')
  
  
  nr <- nr_new(100, 30, 'grey80')
  nr_text(nr, "Hello #RStats", 0, 20, 'black')
  nr_blit(nr, 2, 1, src = dino[[1]])
  dev.hold()
  grid.raster(nr, interpolate = FALSE)
  dev.flush()
  
  start <- Sys.time()
  for (i in -30:110) {
    cat('.')
    nr_fill(nr, 'grey80')
    # nr_text(nr, "Hello #RStats", 0, 20, 'black')
    nr_blit2(nr, i, 1, ss, top_row[[(i %% 32) + 1]])
    dev.hold()
    grid.raster(nr, interpolate = FALSE)
    dev.flush()
    Sys.sleep(0.06)
  }
  Sys.time() - start
}