
alphabet <- c(0:9, LETTERS[1:5])


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
  ints <- str_cols_to_packed_cols(orig)
  final <- packed_cols_to_hex_cols(ints)

  expect_identical(orig, final)
})
