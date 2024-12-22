
test_that("magick round-trip works", {

  skip_if_not_installed('magick')
  
  im <- magick::logo
  nr <- magick_to_nr(im)
  im2 <- nr_to_magick(nr)
  dist <- magick::image_compare_dist(im, im2)
  expect_equal(dist$distortion, 1)

})
