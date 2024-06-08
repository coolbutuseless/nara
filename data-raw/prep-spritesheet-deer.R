

source <- "data-raw/deer female calciumtrice.png"
deer <- png::readPNG(source, native = TRUE)
if (FALSE) {
  grid.newpage(); grid.raster(nr, interpolate = FALSE)
}

ref <- c(0, 0, 32, 32)
locs1 <- lapply(0:4, function(x) {
  new <- ref
  new[1] <- new[1] + x * 32
  new
})


ref <- c(0, 32, 32, 32)
locs2 <- lapply(0:4, function(x) {
  new <- ref
  new[1] <- new[1] + x * 32
  new
})


ref <- c(0, 64, 32, 32)
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

deer_loc <- do.call(rbind, locs)
deer_loc
colnames(deer_loc) <- c('x', 'y', 'w', 'h')

if (FALSE) {
  for (i in seq(nrow(deer_loc))) {
    nr <- nr_new(100, 100, 'grey90')
    nr_blit2(nr, 0, 0, deer, deer_loc[i,])
    grid.newpage(); grid.raster(nr, interpolate = FALSE)
    Sys.sleep(0.3)
  }
}

plot(deer, T)
deer2 <- nr_duplicate(deer)
nr_rect(deer2, 0, 0, 32, 32, fill = NA, color = '#0000ff30')
plot(deer2)


usethis::use_data(deer, deer_loc, internal = FALSE, overwrite = TRUE, compress = 'xz')


