
<!-- README.md is generated from README.Rmd. Please edit that file -->

# nara <img src="man/figures/logo.gif" align="right" width="300" />

<!-- badges: start -->

![](https://img.shields.io/badge/cool-useless-green.svg)
[![R-CMD-check](https://github.com/coolbutuseless/nara/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/coolbutuseless/nara/actions/workflows/R-CMD-check.yaml)
<!-- badges: end -->

`{nara}` provides tools for working with R’s `nativeRaster` image format
to enable fast double-buffered graphics rendering.

### Why?

`nativeRaster` buffers are fast enough to use for rendering at speed
\>30 frames-per-second.  
This makes them useful for games and other interactive applications.

# Details

`{nara}`:

- is an *off-screen* rendering buffer.
- is fast to render.
- uses *in-place* operations to avoid memory allocations.
- is focussed on rendering discrete pixels, so
  - no anti-aliasing is done.
  - all dimensions are rounded to integer values prior to rendering.

### What is a `nativeRaster` and why is it fast?

A `nativeRaster` is a built-in datatype in R.

It is an integer matrix where each integer represents the RGBA color at
a single pixel. The 32-bit integer at each location is interpreted
within R to be four color channels (RGBA) represented by 8 bits each.

This way of encoding color information is closer to the internal
representation used by graphics devices, and therefore can be faster to
render, save and load (as fewer data conversion steps are needed).

### In-place operation

`{nara}` is targeted at fast rendering (\>30fps), and tries to minimise
R function calls and memory allocations.

When updating `nativeRaster` objects with this package, all changes are
done *in place* on the current object i.e. a new object is **not**
created.

### Anti-aliasing/Interpolation

No anti-aliasing is done by the draw methods in this package.

No interpolation is done - `x` and `y` values for drawing coordinates
are converted to integers.

## Installation

You can install from [GitHub](https://github.com/coolbutuseless/nara)
with:

``` r
# install.package('remotes')
remotes::install_github('coolbutuseless/nara')
```

## Vignettes

- [Pacman
  demo](https://coolbutuseless.github.io/package/nara/articles/pacman.html)
- [Creating, transforming, reading, writing nativeRaster
  images](https://coolbutuseless.github.io/package/nara/articles/conversion.html)

## Static Rendering: Example

The following is a rendering of a single scene with multiple elements.

The interesting thing about this scene that drawing all the objects into
the `nativeRaster` image and rendering to screen can take as little as 5
millseconds.

This means that this scene could render at around 200 frames-per-second.

``` r
library(grid)
library(nara)
set.seed(1)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Create 'nr' image
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
w  <- 10
h  <- 8
nr <- nr_new(w * 30, h * 30, fill = 'grey98')

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Draw a grid of squares
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
colors <- viridisLite::inferno(w * h)
coords  <- expand.grid(y = seq(0, h-1) * 30 + 1, x = seq(0, w-1) * 30 + 1)
nr_rect(nr, x = coords$x, y = coords$y, w = 27, h = 27, fill = colors)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Draw a bunch of deer sprites
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_blit2(nr, x = sample(300, 15), y = sample(200, 15), deer, deer_loc[[1]])


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Add an image read from file (with alpha transparency)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
img <- png::readPNG(system.file("img", "Rlogo.png", package="png"), native = TRUE)
nr_blit(nr, 1, 1, img)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Add a polygon
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
thetas <- seq(pi/6, 2*pi, pi/3)
x <- 50 * cos(thetas) + 240
y <- 50 * sin(thetas) + 180
nr_polygon(nr, x = x, y = y, fill = '#556688c0', color = 'blue')

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Add text to the image
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nr_text(nr, x = 180, y = 1, str = "Hello #RStats", fontsize = 16)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copy image to the device
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
grid.raster(nr, interpolate = FALSE)
```

<img src="man/figures/README-unnamed-chunk-3-1.png" width="100%" />

## Static Rendering: Displaying Sprites

Included with `{nara}` are 16 frames of an animated deer character - see
`deer` data.

These frames are consolidated into a single image called a *spritesheet*
and `deer_loc` contains the coordinates of 16 sprites within that image.

#### Blit the first `deer` frame onto a native raster canvas.

``` r
library(grid)

nr <- nr_new(100, 30, 'grey80')
nr_blit2(nr, 2, 1, deer, deer_loc[[1]])
grid.raster(nr, interpolate = FALSE)
```

<img src="man/figures/README-unnamed-chunk-4-1.png" width="100%" />

#### Dynamic (realtime) Rendering: Animated deer

The reason to use `{nara}` is that operations are fast enough that
`nativeRaster` can be used as an *in-memory* buffer for a
double-bufferred rendering system.

`Double-buffered` rendering is where two buffers are used for rendering
with one buffer being shown to the user, and the other existing in
memory as a place to render.

In this example, the `deer` sprite is rendered to a `nativeRaster`
image. This *in-memory* buffer is then displayed to the user using
`grid.raster()`.

By altering the position and animation frame every time the kind is
shown, smooth animation is possible.

This simple code runs at well over 100 frames-per-second.

It is unlikely your screen will refresh this fast, but it does indicate
that there is plenty of headroom for more complicated computations for
each frame.

``` r
library(grid)

# Setup a fast graphics device that can render quickly
x11(type = 'cairo', antialias = 'none')
dev.control('inhibit')

# Create the in-memory nativeRaster canvas
nr <- nr_new(100, 30, 'grey80')

# Clear, blit and render => animation!
for (i in -30:110) {
  nr_fill(nr, 'grey80')                    # Clear the nativeRaster
  nr_blit2(nr, i, 1, deer, deer_loc[[((i/3) %% 5) + 11]]) # copy deer to nativeRaster
  grid.raster(nr, interpolate = FALSE)     # copy nativeRaster to screen
  Sys.sleep(0.03)                          # Stop animation running too fast.
}
```

#### Live screen recording

<img src="man/figures/deer.gif" />

## Multi-Ball

You can quickly *blit* (i.e. copy) a sprite into multiple locations on
the nativeraster with `nr_blit()` and `nr_blit2()`

In this example 100 random positions and velocities are first created. A
character sprite is then blitted to each of these 100 locations.

The positions are updated using the velocities, and the next frame is
rendered. In this way multiple sprites are rendered and animated on
screen.

``` r
library(grid)

# Setup a fast graphics device that can render quickly
x11(type = 'dbcairo', antialias = 'none', width = 8, height = 6)
dev.control('inhibit')

# Number of sprites
N <- 100

# Canvas size
w <- 400 
h <- 300 

# location and movement vector of all the sprites
x  <- sample(w     , N, replace = TRUE)
y  <- sample(h - 25, N, replace = TRUE)
vx <- runif(N, 1, 5)


# Create an empty nativeraster with a grey background
nr <- nr_new(w, h, 'white')


for (frame in 1:1000) {
  # Clear the nativeraster and blit in all the deer
  nr_fill(nr, 'white') 
  nr_blit2(nr, x, y, deer, deer_loc[[ (frame/3) %% 5 + 11]])
  
  # Draw the nativeraster to screen
  dev.hold()
  grid.raster(nr, interpolate = FALSE)
  dev.flush()

  # Update the position of each deer. 
  # Position wraps around
  x <- x + vx
  x <- ifelse(x > w , -32, x)

  # slight pause. Otherwise everything runs too fast!
  Sys.sleep(0.03)
}
```

#### Live screen recording

<img src="man/figures/multiball.gif" />

## Isometric tiling

This package includes `isometric_landscape` which is a collection of 36
isometric tiles (as a list of nativeRasters)

``` r
library(grid)
set.seed(1)

# Set up staggered x coordinates for even/odd rows
even <-  1 + (0:5) * 60
odd  <- 31 + (0:5) * 60

# Sample from just the tree/house tiles.
# Weight the 'basic' tile to occur much more often
tile_idxs <- grep("basic|house|tree", names(isometric_landscape))
probs <- rep(1, length(tile_idxs))
probs[1] <- 30

# A blank cvanas
nr <- nr_new(420, 420, 'white')

# generate isometric tiles from the top down
for (y in seq(0, 350, 30)) {
  
  select <- sample(tile_idxs, length(odd), T, prob = probs)
  for (i in seq_along(odd)) {
    nr_blit(nr,  odd[i], y - 15, isometric_landscape[[select[[i]]]])
  }
  
  select <- sample(tile_idxs, length(even), T, prob = probs)
  for (i in seq_along(even)) {
    nr_blit(nr, even[i], y, isometric_landscape[[select[i]]])
  }
}

grid.newpage()
grid.raster(nr, interpolate = FALSE)
```

<img src="man/figures/README-unnamed-chunk-7-1.png" width="100%" />

## Coordinate System

The coordinate system for `nara` nativeRaster objects has its origins at
the bottom left corner of the image with coordinates `(1,1)`.

<img src="man/figures/coords.png" width="50%" />
