
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Start/end region of masked drawing
#' 
#' These functions are used to globally set a mask
#' @inheritParams nr_rect
#' @param mask Image (cairo raster) to use as mask.  This image does not need 
#'        to be the same size as \code{cr}
#' @return Invisibly return the original \code{cairo raster} object
#' @examplesIf interactive()
#' logo_file <- system.file("img/logo-rainbow.png", package = 'cara', mustWork = TRUE)
#' logo <- nr_read_png(logo_file)
#' plot(logo, T)
#' img <- nr_new(600, 400)
#' 
#' nr_params_set(img, cpar(line_width = 10))
#' 
#' nr_mask_begin(img, mask = logo)
#' nr_circle(img, 300, 200, 200, fill = 'blue', color = 'black')
#' nr_circle(img, 350, 200, 100, fill = 'darkgreen', color = 'black')
#' nr_mask_end(img)
#' 
#' plot(img, T)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_mask_begin <- function(nr, mask) {
  invisible(
    .Call(nr_mask_begin_, nr, mask)
  )
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname nr_mask_begin
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_mask_end <- function(nr) {
  invisible(
    .Call(nr_mask_end_, nr)
  )
}


if (FALSE) {
  logo_file <- system.file("img/logo-rainbow.png", package = 'cara', mustWork = TRUE)
  logo <- fastpng::read_png(logo_file, type = 'nativeraster')
  plot(logo, T)
  img <- nr_new(600, 400)
  
  nr_mask_begin(img, mask = logo)
  nr_circle(img, 300, 200, 200, fill = 'blue', color = 'black')
  nr_circle(img, 350, 200, 100, fill = 'darkgreen', color = 'black')
  nr_mask_end(img)
  
  plot(img, T)
}