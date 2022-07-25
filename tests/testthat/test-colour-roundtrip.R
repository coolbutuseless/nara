
alphabet <- c(0:9, letters[1:5])


make_color <- function(...) {
  paste0(
    '#',
    paste(sample(alphabet, 8, replace = TRUE), collapse = '')
  )
}



test_that("colour roundtrip to ints works", {
  set.seed(2022)
  N <- 1000
  orig <- vapply(1:N, make_color, character(1))
  ints <- colour_to_integer(orig)
  final <- integer_to_colour(ints)

  expect_identical(orig, final)
})
