
test_that("integer matrix to nr works", {
  mat <- matrix(1:24, 8, 3)
  
  nr <- matrix_to_nr(mat, palette = rainbow(24))
  
  expected <-matrix(
    colorfast::col_to_int(rainbow(24))
    , 8, 3, byrow = FALSE
  ) |> t()
  dim(expected) <- c(8, 3) 
  
  expect_equal(nr, expected)
})




test_that("numeric matrix to nr works", {
  mat <- matrix(seq(0, 1, length = 24), 8, 3)
  
  nr <- matrix_to_nr(mat, palette = rainbow(24))
  # plot(nr, T)
  
  colorfast::col_to_int(rainbow(24))
  
  
  expected <-matrix(
    colorfast::col_to_int(rainbow(24))
    , 8, 3, byrow = FALSE
  ) |> t()
  dim(expected) <- c(8, 3) 
  
  expect_equal(nr, expected)
})
