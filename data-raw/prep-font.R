

library(purrr)
library(bdftools)

bdf <- bdftools::read_bdf_builtin('spleen-8x16.bdf')
names(bdf$chars)

chars <- map(bdf$chars[1:95], 'bitmap')


bitmaps <- map_chr(chars, ~paste(.x, collapse = ", "))

cat(bitmaps, sep = ",\n")
