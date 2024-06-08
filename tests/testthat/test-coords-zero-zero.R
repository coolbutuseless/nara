



test_that("zero-zero at top-left - point", {
  
  nr <- nr_new(3, 2)
  nr_point(nr, 0, 0, -2L)
  expect_identical(  
    as.vector(nr), 
    c(
      -2L, -1L, -1L, 
      -1L, -1L, -1L
    )
  )
  
  
  nr <- nr_new(3, 2)
  nr_point(nr, 1, 0, -2L)
  expect_identical(  
    as.vector(nr), 
    c(
      -1L, -2L, -1L, 
      -1L, -1L, -1L
    )
  )
  
  
  nr <- nr_new(3, 2)
  nr_point(nr, 1, 1, -2L)
  expect_identical(  
    as.vector(nr), 
    c(
      -1L, -1L, -1L, 
      -1L, -2L, -1L
    )
  )
  
  
  nr <- nr_new(3, 2)
  nr_point(nr, 2, 2, -2L)
  expect_identical(  
    as.vector(nr), 
    c(
      -1L, -1L, -1L, 
      -1L, -1L, -1L
    )
  )
})




test_that("zero-zero at top-left - rect", {
  
  nr <- nr_new(4, 3)
  nr_rect(nr, 0, 0, 2, 2, fill = -2L)
  nr
  expect_identical(  
    as.vector(nr), 
    c(
      -2L, -2L, -1L, -1L, 
      -2L, -2L, -1L, -1L,
      -1L, -1L, -1L, -1L
    )
  )
  
  
  nr <- nr_new(4, 3)
  nr_rect(nr, 1, 1, 2, 2, fill = -2L)
  # nr
  expect_identical(  
    as.vector(nr), 
    c(
      -1L, -1L, -1L, -1L, 
      -1L, -2L, -2L, -1L,
      -1L, -2L, -2L, -1L
    )
  )
  
  
  nr <- nr_new(4, 3)
  nr_rect(nr, 2, 2, 2, 2, fill = -2L)
  # nr
  expect_identical(  
    as.vector(nr), 
    c(
      -1L, -1L, -1L, -1L,
      -1L, -1L, -1L, -1L,
      -1L, -1L, -2L, -2L
    )
  )
  
  if (FALSE) {
    nr <- nr_new(4, 3) |>
      nr_rect(2, 2, 2, 2, fill = 'black') |>
      plot(T)
    nr
  }
  
  
})
