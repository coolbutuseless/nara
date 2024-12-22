

set.seed(2022)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Conversion to/from raster
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
test_that("round trip from raster works", {
  w <- 15
  h <- 10
  mat   <- matrix(rainbow(w*h), h, w)
  mat[] <- paste0(mat, "FF")
  ras   <- as.raster(mat)
  
  nr   <- raster_to_nr(ras)
  ras2 <- nr_to_raster(nr)
  expect_identical(ras, ras2)
})

