

test_that("NN scale works", {
  
  black <- colorfast::col_to_int('black')
  clear <- colorfast::col_to_int('#00000000')
  half  <- colorfast::col_to_int('#0000007f')
  
  nr <- nr_new(2, 2, fill = clear)
  nr_point(nr, 0, 0, color = black)
  nr_point(nr, 1, 1, color = black)

  nr3 <- nr_scale(nr, 3, algo = 'nn')  
  expect_equal(nrow(nr3), 6)  
  expect_equal(ncol(nr3), 6)  
  
  expect_equal(
    nr3, 
    matrix(c(
      black, black, black, clear, clear, clear,
      black, black, black, clear, clear, clear,
      black, black, black, clear, clear, clear,
      clear, clear, clear, black, black, black,
      clear, clear, clear, black, black, black,
      clear, clear, clear, black, black, black),
      nrow = 6, ncol = 6, byrow = TRUE
    )
  )
  
  
  
  nr3 <- nr_scale(nr, 1.5, algo = 'bilinear')  
  expect_equal(nrow(nr3), 3)  
  expect_equal(ncol(nr3), 3)  
  
  expect_equal(
    nr3,
    matrix(c(
      black, half, clear,
      half, half, half,
      clear, half, black
    ), nrow = 3, ncol = 3, byrow = TRUE)
  )
    
})
