
test_that("nr-draw line works", {
  
  nr <- nr_new(5, 5, 'white')
  
  nr_line(nr, 0, 0, 4, 4)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('white')
  
  expect_equal(
    nr,
    matrix(c(
      black, white, white, white, white,
      white, black, white, white, white,
      white, white, black, white, white,
      white, white, white, black, white,
      white, white, white, white, black
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
  
})



test_that("nr-draw rect works", {
  
  nr <- nr_new(5, 5, 'white')
  nr_rect(nr, 0, 0, 2, 2)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('white')
  
  expect_equal(
    nr,
    matrix(c(
      black, black, white, white, white,
      black, black, white, white, white,
      white, white, white, white, white,
      white, white, white, white, white,
      white, white, white, white, white
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
  
})


test_that("nr-draw rect works - justify 0,0", {
  
  nr <- nr_new(5, 5, 'white')
  nr_rect(nr, 2, 2, 3, 3, hjust = 0, vjust = 0)
  # plot(nr, T)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('white')
  
  expect_equal(
    nr,
    matrix(c(
      white, white, white, white, white,
      white, white, white, white, white,
      white, white, black, black, black,
      white, white, black, black, black,
      white, white, black, black, black
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
  
})


test_that("nr-draw rect works - justify 1,1", {
  
  nr <- nr_new(5, 5, 'white')
  nr_rect(nr, 2, 2, 3, 3, hjust = 1, vjust = 1)
  # plot(nr, T)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('white')
  
  expect_equal(
    nr,
    matrix(c(
      black, black, black, white, white,
      black, black, black, white, white,
      black, black, black, white, white,
      white, white, white, white, white,
      white, white, white, white, white
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
  
})


test_that("nr-draw rect works - justify 0.5, 0.5", {
  
  nr <- nr_new(5, 5, 'white')
  nr_rect(nr, 2, 2, 3, 3, hjust = 0.5, vjust = 0.5)
  # plot(nr, T)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('white')
  
  expect_equal(
    nr,
    matrix(c(
      white, white, white, white, white,
      white, black, black, black, white,
      white, black, black, black, white,
      white, black, black, black, white,
      white, white, white, white, white
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
  
})


test_that("nr-draw rect works - justify 0.5, 0.5 border", {
  
  nr <- nr_new(5, 5, 'white')
  nr_rect(nr, 2, 2, 3, 3, hjust = 0.5, vjust = 0.5, color = 'red')
  # plot(nr, T)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('white')
  red   <- colorfast::col_to_int('red')
  
  expect_equal(
    nr,
    matrix(c(
      white, white, white, white, white,
      white, red  , red  , red  , white,
      white, red  , black, red  , white,
      white, red  , red  , red  , white,
      white, white, white, white, white
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
})




test_that("nr-draw circle works", {
  
  nr <- nr_new(5, 5, 'white')
  nr_circle(nr, 2, 2, 2)
  # plot(nr, T)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('white')
  red   <- colorfast::col_to_int('red')
  
  expect_equal(
    nr,
    matrix(c(
      white, black, black, black, white,
      black, black, black, black, black,
      black, black, black, black, black,
      black, black, black, black, black,
      white, black, black, black, white
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
})


test_that("nr-draw circle works with border", {
  
  nr <- nr_new(5, 5, 'white')
  nr_circle(nr, 2, 2, 2, col = 'red')
  # plot(nr, T)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('white')
  red   <- colorfast::col_to_int('red')
  
  expect_equal(
    nr,
    matrix(c(
      white, red  , red  , red  , white,
      red  , black, black, black, red  ,
      black, black, black, black, black,
      red  , black, black, black, red  ,
      white, red  , red  , red  , white
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
})





test_that("nr-draw polyline works", {
  
  nr <- nr_new(5, 5, 'white')
  nr_polyline(nr, c(0, 0, 4), c(4, 0, 0))
  # plot(nr, T)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('white')
  red   <- colorfast::col_to_int('red')
  
  expect_equal(
    nr,
    matrix(c(
      black, black, black, black, black,
      black, white, white, white, white,
      black, white, white, white, white,
      black, white, white, white, white,
      black, white, white, white, white
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
})




test_that("nr-draw polyline works with close", {
  
  nr <- nr_new(5, 5, 'white')
  nr_polyline(nr, c(0, 0, 4), c(4, 0, 0), close = TRUE)
  # plot(nr, T)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('white')
  red   <- colorfast::col_to_int('red')
  
  expect_equal(
    nr,
    matrix(c(
      black, black, black, black, black,
      black, white, white, black, white,
      black, white, black, white, white,
      black, black, white, white, white,
      black, white, white, white, white
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
})



test_that("nr-draw polygon works", {
  
  nr <- nr_new(5, 5, 'white')
  # nr_polygon(nr, c(0, 0, 4), c(4, 0, 0))
  nr_polygon(nr, c(4, 0, 0), c(0, 0, 4))
  if (interactive()) plot(nr, T)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('white')
  red   <- colorfast::col_to_int('red')
  
  expect_equal(
    nr,
    matrix(c(
      black, black, black, black, white,
      black, black, black, black, white,
      black, black, black, white, white,
      black, black, white, white, white,
      black, white, white, white, white
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
})


test_that("nr-draw polygon works with outline", {
  
  nr <- nr_new(5, 5, 'white')
  nr_polygon(nr, c(4, 0, 0), c(0, 0, 4), color = 'green')
  if (interactive()) plot(nr, T)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('white')
  green <- colorfast::col_to_int('green')
  
  expect_equal(
    nr,
    matrix(c(
      green, green, green, green, green,
      green, black, black, green, white,
      green, black, green, white, white,
      green, green, white, white, white,
      green, white, white, white, white
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
})


test_that("nr-draw text works", {
  
  nr <- nr_new(width = 5, height = 5, 'azure')
  nr_text_basic(nr, 0, 5, ".")
  if (interactive()) plot(nr, T)
  
  black <- colorfast::col_to_int('black')
  white <- colorfast::col_to_int('azure')
  green <- colorfast::col_to_int('green')
  
  expect_equal(
    nr,
    matrix(c(
      white, white, white, white, white,
      white, white, white, white, white,
      white, white, white, white, white,
      white, white, white, black, white,
      white, white, white, white, white
    ), 5, 5, byrow = T), ignore_attr = TRUE
  )
})








