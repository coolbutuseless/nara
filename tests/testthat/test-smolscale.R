
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Exercise the library toensure there are no memory errors when 
# used with R.
# Any such errors will hopefully be picked up the test builds for:
#   clang-asan, rchk, valgrind
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
test_that("smolscale works", {

  nr <- nr_duplicate(deer_sprites[[1]])
  for (w in c(10, 100, 400, 1000)) {
    expect_no_error({
      nr_resize(nr, w, w) 
    })
  }

  
  orders <- c('RGBA', 'BGRA', 'ARGB', 'ABGR')
  
  
  for (src_order in orders) {
    for (target_order in orders) {
      for (src_pm_alpha in c(TRUE, FALSE)) {
        for (target_pm_alpha in c(TRUE, FALSE)) {
          expect_no_error({
            nr <- nr_duplicate(deer_sprites[[1]])
            nr_swizzle(nr, order = src_order, pm_alpha = src_pm_alpha, 
                       target_order = target_order, target_pm_alpha = target_pm_alpha) 
          })
        }
      }
    }
  }
})
