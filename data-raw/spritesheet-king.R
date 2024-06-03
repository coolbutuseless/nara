

source <- "data-raw/oga-buch/characters_7.png"
king <- png::readPNG(source)
if (FALSE) {
  grid.newpage(); grid.raster(ss)
}

ref <- c(7, 65, 18, 24)
king_loc <- lapply(0:15, function(x) {
  new <- ref
  new[1] <- new[1] + x * 32
  new
})


for (loc in king_loc) {
  nr <- nr_new(100, 100, 'grey90')
  nr_blit2(nr, 1, 1, king, loc)
  grid.newpage(); grid.raster(nr, interpolate = FALSE)
  Sys.sleep(0.3)
}


# usethis::use_data(king, king_locs, internal = FALSE, overwrite = TRUE, compress = 'xz')

