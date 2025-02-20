% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/nr-draw.R
\name{nr_polygon}
\alias{nr_polygon}
\title{Draw multiple polygon on a \code{nativeRaster} image}
\usage{
nr_polygon(
  nr,
  x,
  y,
  id = NULL,
  fill = "black",
  color = NA,
  linewidth = 1,
  mitre_limit = linewidth,
  mode = draw_mode$respect_alpha
)
}
\arguments{
\item{nr}{\code{nativeRaster}}

\item{x, y}{Vectors of point coordinates}

\item{id}{integer vector used to separate coordinates into 
multiple polygons. Consecutive runs of the same \code{id}
value belong to the same polygon.  If NULL (the default) then 
all coordinates are assumed to be vertices of a single polygon.}

\item{fill}{fill color}

\item{color}{Color as a character string. Either a standard R color
(e.g. 'blue', 'white')
or a hex color of the form \code{#rrggbbaa}, \code{#rrggbb}, \code{#rgba}
or \code{#rgb}}

\item{linewidth}{Line linewidth. Default: 1.  If \code{linewidth = 1} then a 
naive version of Bresenham is used to draw the points.  If linewidth 
is greater than 1, then the line is convert to a triangle strip and
rendered as polygons.}

\item{mitre_limit}{Limit the size of the mitre when two lines meet at an 
acute angle and linewidth is greater than 1. Default: same as line 
linewidth which mostly looks OK.}

\item{mode}{drawing mode.}
}
\value{
Original \code{nativeRaster} modified in-place
}
\description{
Draw multiple polygon on a \code{nativeRaster} image
}
\examples{
N <- 200
nr <- nr_new(N, N, 'grey80')
nr_polygon(nr, x = c(10, N-10, 10), y = c(10, 10, N-10), fill = 'blue', 
           color = 'red', linewidth = 5)
plot(nr, TRUE)

}
