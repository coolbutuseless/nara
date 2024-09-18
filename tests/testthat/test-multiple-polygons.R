

test_that("multiple polygons work", {

  x <- c(0, 100, 100)
  y <- c(0,   0, 100)
  
  nr <- nr_new(100, 100)
  
  expect_no_error(
    nr_polygons(nr, x = x, y = y)
  )
  
  x <- c(0, 100, 100,   0,  0,  100,   25, 75, 75, 25)
  y <- c(0,   0, 100,   0, 100, 100,   25, 25, 75, 75)
  id <- c(1, 1, 1, 2, 2, 2, 8, 8, 8, 8)
  
  nr <- nr_new(100, 100)
  
  expect_no_error(
    nr_polygons(nr, x = x, y = y, id = id, fill = c('grey20', 'blue', 'hotpink'))
  )  
})
