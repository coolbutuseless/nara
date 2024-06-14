

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert a 'magick' image to \code{nativeRaster}
#' 
#' @inheritParams matrix_to_nr
#' @inheritParams array_to_nr
#' @param im image from the \code{magick} package
#' 
#' @return \code{nativeRaster}
#' 
#' @examples
#' if (requireNamespace('magick', quietly = TRUE)) {
#'   im <- magick::image_read(system.file("img/Rlogo.png", package = "png"))
#'   nr <- magick_to_nr(im)
#'   plot(nr)
#' }
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
magick_to_nr <- function(im, dst = NULL) {
  if (requireNamespace('magick', quietly = TRUE)) {
    .Call(magick_to_nr_, magick::image_data(im), dst)
  } else {
    message("Please install the 'magick' package in order to use this function");
    NULL
  }
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname magick_to_nr
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_to_magick <- function(nr) {
  if (requireNamespace('magick', quietly = TRUE)) {
    magick::image_read(nr)
  } else {
    message("Please install the 'magick' package in order to use this function");
    NULL
  }
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Magick conversion - possibly won't be in final version of 'nara' pkg
#' 
#' @param nr_list list of nativeRasters
#' @param gif_name name of mp4 file to save
#' @param verbose logical. default FALSE
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nrs_to_gif <- function(nr_list, gif_name, verbose = FALSE) {
  if (requireNamespace('magick', quietly = TRUE)) {
    nr_list <- lapply(nr_list, nr_to_magick)
    ims <- do.call(magick::image_join, nr_list)
    magick::image_write_gif(ims, path = gif_name, progress = verbose)
  } else {
    stop("need to install {magick}")
  }
  invisible()
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Magick conversion - possible won't be in final version of 'nara' pkg
#' 
#' @param nr_list list of nativeRasters
#' @param mp4_name name of mp4 file to save
#' @param verbose logical. default FALSE
#' 
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nrs_to_mp4 <- function(nr_list, mp4_name, verbose = FALSE) {
  if (requireNamespace('magick', quietly = TRUE)) {
    nr_list <- lapply(nr_list, nr_to_magick)
    ims <- do.call(magick::image_join, nr_list)
    magick::image_write_video(ims, path = mp4_name, verbose = verbose)
  } else {
    stop("need to install {magick}")
  }
  invisible()
}