

source <- "data-raw/deer female calciumtrice.png"
deer <- read_png(source)
# ss <- read_png(source, transparent = list(-1594494))
if (FALSE) {
  grid.newpage(); grid.raster(nr, interpolate = FALSE)
}

ref <- c(1, 129, 32, 32)
locs1 <- lapply(0:4, function(x) {
  new <- ref
  new[1] <- new[1] + x * 32
  new
})


ref <- c(1, 97, 32, 32)
locs2 <- lapply(0:4, function(x) {
  new <- ref
  new[1] <- new[1] + x * 32
  new
})



ref <- c(1, 65, 32, 32)
locs3 <- lapply(0:4, function(x) {
  new <- ref
  new[1] <- new[1] + x * 32
  new
})

locs <- c(locs1, locs2, locs3)
names(locs) <- c(
  'idle01', 'idle02', 'idle03', 'idle04', 'idle05', 
  'idle06', 'idle07', 'idle08', 'idle09', 'idle10',
  'run1', 'run2', 'run3', 'run4', 'run5'
)

if (FALSE) {
  for (loc in locs) {
    nr <- nr_new(100, 100, 'grey90')
    nr_blit2(nr, 1, 1, deer, loc)
    grid.newpage(); grid.raster(nr, interpolate = FALSE)
    Sys.sleep(0.3)
  }
}

deer_loc <- locs
usethis::use_data(deer, deer_loc, internal = FALSE, overwrite = TRUE, compress = 'xz')


stop("done")

ss <- deer
x <- 7
y <- 44

w <- 100
h <- 100

w_sprite <- 32
h_sprite <- 32

plot.new()

xoff <-  20
yoff <-  -20

while (TRUE) {
  nr <- nr_new(w, h, 'grey90')
  nr_blit(nr, xoff, yoff, ss)
  nr_rect(nr, x-1, y-1, w_sprite+2, h_sprite+2, fill = NA, color = '#00000020')
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
    xoff <- xoff + 32
  } else if (loc$x >= 0.4 && loc$y >= -0.05 && loc$y <= 0.05) {
    xoff <- xoff - 32
  } else if (loc$y <= -0.4 && loc$x >= -0.05 && loc$x <= 0.05) {
    yoff <- yoff + 32
  } else if (loc$y >= 0.4 && loc$x >= -0.05 && loc$x <= 0.05) {
    yoff <- yoff - 32
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
