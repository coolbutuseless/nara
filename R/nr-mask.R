
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Start/end region of masked drawing
#' 
#' These functions are used to globally set a mask which determines where
#' pixels are affected by drawing functions.
#' 
#' Drawing operations between
#' \code{nr_mask_begin()} and \code{nr_mask_end()} calls will only affect pixels
#' where the \code{mask} is not transparent.
#' 
#' @inheritParams nr_fill
#' @param mask native raster image to use as mask. Must be the same size as
#'        \code{nr}.
#'        
#' @return Invisibly return the original native raster image which has been 
#'         modified in-place
#'         
#' @examplesIf interactive()
#' mask <- deer[[1]] |> nr_resize(600, 400)
#' plot(mask)
#' img <- nr_new_from(mask)
#' 
#' nr_mask_begin(img, mask = mask)
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

