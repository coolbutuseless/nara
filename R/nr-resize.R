
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Resize with interpolation
#' 
#' @param nr native raster
#' @param width,height target size
#' 
#' @return new nativeraster
#' @examples
#' stretched_deer <- nr_resize(deer_sprites[[1]], 200, 32)
#' plot(stretched_deer, TRUE)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_resize <- function(nr, width, height) {
  .Call(resize_, nr, width, height)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Swizzle the pixel ordering
#' 
#' @param nr native raster
#' @param order Order of source pixels. Native rasters are 'RGBA'. Possible values:
#'        'RGBA', 'BGRA', 'ARGB', 'ABGR'
#' @param pm_alpha Is the alpha pre-multiplied? Default: FALSE matches how
#'        native rasters are stored.
#' @param target_order Order of output pixels
#' @param target_pm_alpha should the final alpha be pre-multiplied? Default: FALSE
#' 
#' @return Original raster modified in place and returned invisibly
#' @examples
#' deer <- deer_sprites[[1]]
#' plot(deer, T)
#' nr_swizzle(deer, target_order = 'BGRA')
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_swizzle <- function(nr, order = 'RGBA', pm_alpha = FALSE, target_order, target_pm_alpha = FALSE) {
  invisible(
    .Call(swizzle_, nr, order, pm_alpha, target_order, target_pm_alpha)
  )
}



if (FALSE) {
  
  logo <- png::readPNG(system.file("img", "Rlogo.png", package="png"), native = TRUE)
  # plot(logo, T)
  nr_resize(logo, 1600, 500) |> bench::mark()
  l2 <- nr_resize(logo, 1600, 500) #|> bench::mark()
  plot(l2, T)
  
  library(magick)
  im <- nr_to_magick(logo)
  image_resize(im, geometry = geometry_size_pixels(1600, 500, preserve_aspect = FALSE))  |> bench::mark()
  
  
  library(nara)
  library(naracairo)

  
  l1 <- png::readPNG(system.file("img", "Rlogo.png", package="png"), native = TRUE)
  l2 <- nr_duplicate(l1)
  # nr_swizzle(l2, target_order = 'BGRA') |> bench::mark()
  nr_swizzle(l2, target_order = 'BGRA') 
  plot(l2, T)
  
  l3 <- nr_duplicate(l1)
  cr <- init_cairo(l3)
  # naracairo::begin_cairo(cr) |> bench::mark()
  naracairo::begin_cairo(cr) 
  plot(l3, T)
    
}