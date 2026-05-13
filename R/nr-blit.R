

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Copy a native raster image into another at an arbitrary location, scale and rotation
#'
#' Copy a whole image, or subset of an image, into the destination image and 
#' configure the angle and size of the copied image.
#'
#' This operation is vectorised such that a single \code{src} image can be 
#' pasted multiple times into the \code{dst} image with varying location, 
#' angle and scale.
#'
#' @inheritParams nr_point
#' @param dst,src source and destination native raster images
#' @param x,y Location in \code{dst} to place the \code{src} image. These values must
#'        be vectors of the same length.  If the length is greater than 1, then
#'        the \code{src} will be pasted into \code{dst} at multiple locations.
#' @param xsrc,ysrc Upper-left coordinates of the bounding box within the \code{src}
#'        image to copy.  Default: (0, 0)
#' @param w,h Width and height of the bounding box within the \code{src} to copy.
#'        If size is negative, then the actual width/height of
#'        the src is used. Default: (-1, -1)
#' @param hjust,vjust specify horizontal and vertical justification of the 
#'        handle on the \code{src} image to be placed at \code{(x, y)} within 
#'        the \code{dst} image.
#'        Default (0, 0) is to use the top-left corner of the image as the 
#'        handle.
#'        Use (0.5, 0.5) to centre the \code{src} image at the \code{dst} 
#'        location \code{(x, y)}
#' @param angle Rotation angle (clockwise) in radians. Default: 0.
#'        If this is a vector, then the \code{src} will be pasted into the
#'        \code{dst} at multiple angles.
#' @param scale Zoom factor. Default: 1.  IF this is a vector, then the \code{src}
#'        will be pasted into \code{dst} at multiple scales.
#'
#' @return Invisibly return the supplied \code{dst} native raster image which was been
#'         modified in-place
#' 
#' @examples
#' nr <- nr_new(50, 50, 'grey80')
#' nr_blit(dst = nr, src = deer[[1]], x = 25, y = 25)
#' plot(nr)
#' 
#' nr <- nr_new(300, 200, 'grey80')
#' sq <- nr_new(20, 20, 'darkblue')
#' nr_blit(nr, src = sq, x = 100, y = 100, angle = pi/3, scale = 5)
#' plot(nr)
#'
#' nr <- nr_new(800, 600, 'grey80')
#' sq <- fastpng::read_png(system.file("image/deer-1.png", package="nara"), type = 'nativeraster')
#' nr_blit(nr, src = sq, x = 300, y = 240, angle = pi/2, scale = 1)
#' plot(nr)
#' @family blitting functions
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit <- function(dst, src, x, y, 
                    xsrc = 0L, ysrc = 0L, 
                    w = -1L, h = -1L,
                    hjust = 0.5, vjust = 0.5, 
                    angle = 0, scale = 1,
                    use_alpha = TRUE) {
  invisible(
    .Call(nr_blit_, 
          dst, x, y, 
          src, xsrc, ysrc, 
          w, h,
          hjust, vjust,
          angle, scale, 
          use_alpha)
  )
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Multiple blit operations in a single call
#' 
#' @param dst destination native raster
#' @param src list of native raster images.
#' @param config data.frame of configuration information for each blit.
#'    Columns in this data.frame match the arguments to \code{nr_blit()}.
#'    Requires columns:
#'    \describe{
#'      \item{\code{idx}}{The index of the image in the \code{src} list i.e. \code{src[[idx]]}}
#'      \item{\code{x,y}}{location in \code{dst} to place \code{src[[idx]]}}
#'    } 
#'    Optional columns which take a default value if not present:
#'    \describe{
#'      \item{\code{xsrc,ysrc}}{start coordinates within src. Default: (0, 0)}
#'      \item{\code{w,h}}{size within \code{src}. Default: use whole width/height}
#'      \item{\code{hjust,vjust}}{Default: (0.5, 0.5)}
#'      \item{\code{angle}}{Default: 0}
#'      \item{\code{scale}}{Zoom factor. Default: 1}
#'      \item{\code{use_alpha}}{Default: true}
#'      \item{\code{render}}{Should this given operation be rendered? Default: true}
#'    }
#'    
#' @return Invisibly return the supplied \code{dst} native raster image which was been
#'         modified in-place
#'           
#' @examples
#' nr <- nr_new(90, 90, 'grey60')
#' 
#' config <- data.frame(
#'   idx = c(1, 2, 3, 4),
#'   x = c(10, 10, 40, 40) + 15,
#'   y = c(10, 40, 40, 10) + 15,
#'   xsrc = 0L,
#'   ysrc = 0L,
#'   w = -1L,
#'   h = -1L,
#'   hjust = 0.5,
#'   vjust = 0.5,
#'   angle = c(0, 0, 0, pi/4),
#'   scale = c(0.5, 1, 1, 1),
#'   use_alpha = TRUE,
#'   render = TRUE
#' )
#' 
#' nr_blit_multi(dst = nr, src = deer, config = config)
#' plot(nr)
#' @family blitting functions
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit_multi <- function(dst, src, config) {
  invisible(
    .Call(nr_blit_multi_, dst, src, config)
  )
}


