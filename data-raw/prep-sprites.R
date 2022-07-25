
library(grid)

x11(type = 'cairo')

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Read in the sprite sheet
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
sprite <- png::readPNG("data-raw/bubble-bobble.png", native = FALSE)

sub1 <- sprite[118:142,0:455,]
grid.newpage()
grid.raster(sub1, interpolate = FALSE)

off <- c(5, 34, 64, 93, 121, 147, 175, 202, 232, 260, 289, 317, 345, 372, 401, 430)

for (i in c(1:16)) {
  sub <- sub1[,0:23 + off[i],]
  grid.raster(sub, interpolate = FALSE)
  # Sys.sleep(0.5)

  png::writePNG(sub, sprintf("data-raw/dino/%02i.png", i))
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Make background transparent
#                 Blue BG     Transparent White
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
dino <- lapply(list.files("data-raw/dino/", full.names = TRUE), \(x) {
  sprite <- png::readPNG(x, native = TRUE)
  sprite[sprite == -1262701] <- colour_to_integer('#ffffff00')
  sprite
})


usethis::use_data(dino, internal = FALSE, overwrite = TRUE, compress = 'xz')
