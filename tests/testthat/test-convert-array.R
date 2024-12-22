

set.seed(2022)


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Round trip to raster (with alpha)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
test_that("round trip from array works (with alpha)", {
  
  im <- testthat::test_path("image/Rlogo-tiny.png")
  arr <- png::readPNG(im)
  # plot(as.raster(arr), interpolate = FALSE)

  nr <- array_to_nr(arr)
  # plot(nr, T)
  
  arr2 <- nr_to_array(nr)
  # plot(as.raster(arr2), interpolate = FALSE)

  attr(arr2, "class") <- NULL
  
  expect_identical(arr, arr2)
})


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Round trip to raster (without alpha)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
test_that("round trip from array works (without alpha)", {
  
  im <- testthat::test_path("image/Rlogo-tiny.png")
  arr <- png::readPNG(im)
  arr <- arr[,,1:3]
  # plot(as.raster(arr), interpolate = FALSE)
  
  nr <- array_to_nr(arr)
  # plot(nr, T)
  
  arr2 <- nr_to_array(nr)
  # plot(as.raster(arr2), interpolate = FALSE)
  
  attr(arr2, "class") <- NULL
  
  expect_identical(arr, arr2[,,1:3])
})

