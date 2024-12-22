

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Convert a 'magick' image to \code{nativeRaster}
#' 
#' @inheritParams matrix_to_nr
#' @inheritParams array_to_nr
#' @param im image from the \code{magick} package
#' 
#' @return \code{nativeRaster}
#' 
#' @examplesIf interactive() && requireNamespace('magick', quietly = TRUE)
#' im <- magick::logo
#' nr <- magick_to_nr(im)
#' plot(nr)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
magick_to_nr <- function(im, dst = NULL) {
  if (requireNamespace('magick', quietly = TRUE)) {
    im <- magick::image_convert(im, format = 'rgba', matte = TRUE)
    if (length(im) == 1) {
      .Call(magick_to_nr_, magick::image_data(im), dst)
    } else {
      lapply(seq_along(im), function(i) {
        .Call(magick_to_nr_, magick::image_data(im, frame = i), dst)
      })
    }
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
#' @param filename filename of gif
#' @param verbose logical. default FALSE
#' @param framerate frames per second
#' @param ... other arguments passed to \code{magick::image_write_gif()}
#' @return None.
#' @examplesIf interactive() && requireNamespace('magick', quietly = TRUE)
#' im <- magick::logo
#' nr1 <- magick_to_nr(im)
#' nr2 <- nr_duplicate(nr1)
#' nrs_to_gif(list(nr1, nr2), "nothing.gif")
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nrs_to_gif <- function(nr_list, filename, verbose = FALSE, framerate = 30, ...) {
  if (requireNamespace('magick', quietly = TRUE)) {
    nr_list <- lapply(nr_list, nr_to_magick)
    ims <- do.call(magick::image_join, nr_list)
    magick::image_write_gif(ims, path = filename, progress = verbose, delay = 1/framerate, ...)
  } else {
    stop("need to install {magick}")
  }
  invisible()
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Magick conversion - possible won't be in final version of 'nara' pkg
#' 
#' @param nr_list list of nativeRasters
#' @param filename mp4 filename
#' @param verbose logical. default FALSE
#' @param ... other arguments passed to \code{magick::image_write_video()}
#' @return None.
#' @examplesIf interactive() && requireNamespace('magick', quietly = TRUE)
#' im <- magick::logo
#' nr1 <- magick_to_nr(im)
#' nr2 <- nr_duplicate(nr1)
#' nrs_to_mp4(list(nr1, nr2), "nothing.mp4")
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nrs_to_mp4 <- function(nr_list, filename, verbose = FALSE, ...) {
  if (requireNamespace('magick', quietly = TRUE)) {
    nr_list <- lapply(nr_list, nr_to_magick)
    ims <- do.call(magick::image_join, nr_list)
    magick::image_write_video(ims, path = filename, verbose = verbose, ...)
  } else {
    stop("need to install {magick}")
  }
  invisible()
}
