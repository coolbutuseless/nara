

test_that("Crop work works", {

  nr <- nr_new(400, 400, 'hotpink')
  nr2 <- nr_crop(nr, 0, 0, 4, 3)
  expect_equal(ncol(nr2), 4)
  expect_equal(nrow(nr2), 3)
  
  expect_identical(unique(nr2), colorfast::col_to_int('hotpink'))
})
