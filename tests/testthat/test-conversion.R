


set.seed(2022)

w <- 30
h <- 20

alphabet <- c(0:9, letters[1:5])


make_color <- function(...) {
  paste0(
    '#',
    paste(sample(alphabet, 8, replace = TRUE), collapse = '')
  )
}


cols <- vapply(seq(w*h), make_color, character(1))
ras0 <- matrix(cols, nrow = h, ncol = w)

nr1 <- raster_to_nr(ras0)
ras2 <- nr_to_raster(nr1)

arr3 <- nr_to_array(nr1)
nr4  <- array_to_nr(arr3)


arr5 <- png::readPNG("image/Rlogo-tiny.png")
nr6  <- array_to_nr(arr5)
arr7 <- nr_to_array(nr6)

# Convert from array-to-raster-to-nativeraster and back again
# and check everything agrees
test_that("formatting round-trip works", {
  expect_identical(ras0, ras2)
  expect_identical(nr1, nr4)
  expect_identical(arr5, arr7)
})
