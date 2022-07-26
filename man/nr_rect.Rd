% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/nativeraster.R
\name{nr_rect}
\alias{nr_rect}
\title{Draw rectangles on a \code{nativeRaster} image}
\usage{
nr_rect(nr, x, y, w, h, fill = "black", colour = NA)
}
\arguments{
\item{nr}{\code{nativeRaster} image}

\item{x, y}{coordinates of lower left corner of rectangle.  [vector]}

\item{w, h}{width and height of rectangle. [vector]}

\item{fill}{interior fill colour [vector]}

\item{colour}{outline colour. Default: NA. [vector]}
}
\description{
Draw rectangles on a \code{nativeRaster} image
}
