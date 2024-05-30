
library(png)

files <- list.files("data-raw/Tiles/", pattern = 'png', full.names = TRUE)

isometric_landscape <- files |> 
  lapply(\(x) png::readPNG(x, native = TRUE)) |>
  setNames(tools::file_path_sans_ext(basename(files)))

length(isometric_landscape)


usethis::use_data(isometric_landscape, internal = FALSE, overwrite = TRUE, compress = 'xz')
