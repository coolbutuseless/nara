
fns <- list.files("data-raw/tileset/", full.names = TRUE)

fn <- fns[[1]]


nms <- tools::file_path_sans_ext(basename(fns))
nms


tileset <- lapply(fns, function(fn) {
  fastpng::read_png(fn, type = 'nativeraster')
})

names(tileset) <- nms
usethis::use_data(tileset, internal = FALSE, overwrite = TRUE, compress = 'xz')



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# 
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
w <- 70 * 11
h <- 70 * 8
nr <- nr_new(w, h, fill = 'lightblue')
config1 <- readr::read_csv(
"name,x,y
cloud3, 0, 0.5
cloud3, 0.5, 0.5
cloud3, 10, 1

brickWall  , 1, 2
boxItemAlt , 2, 2

brickWall  , 6, 0.5
boxItemAlt , 7, 0.5
brickWall  , 8, 0.5


mushroomRed, 1, 5
hill_small , 2, 4.5
brickWall  , 3, 5

grassLeft  , 0, 6
grassLeft  , 1, 6
grassLeft  , 2, 6
grassLeft  , 3, 6
grassLeft  , 0, 7
grassLeft  , 1, 7
grassCenter, 2, 7
grassCenter, 3, 7


liquidWater, 4, 7
liquidWater, 5, 7
liquidWater, 6, 7
liquidWaterTop_mid, 4, 6
liquidWaterTop_mid, 5, 6
liquidWaterTop_mid, 6, 6
fishSwim1, 5.8, 7


grassCenter,  7, 7
grassCenter,  8, 7
grassCenter,  9, 7
grassCenter, 10, 7
grassCenter,  7, 6
grassCenter,  8, 6
grassCenter,  9, 6
grassCenter, 10, 6
grassCenter,  7, 5
grassCenter,  8, 5
grassCenter,  9, 5
grassCenter, 10, 5
grassCliffLeft, 6, 4
grassMid,  7, 4
grassCenter,  8, 4
grassCenter,  9, 4
grassCenter, 10, 4

door_closedMid, 9, 3
door_closedTop, 9, 2

plant, 6, 3
plant, 7, 3

star,  8, 3
star,  8, 2
star, 10, 3
star, 10, 2

")

config1$x <- config1$x * 70
config1$y <- config1$y * 70
config1$idx <- match(config1$name, names(tileset))
config1$hjust <- 0
config1$vjust <- 0
config1


tileset_config <- config1
usethis::use_data(tileset_config, internal = FALSE, overwrite = TRUE, compress = 'xz')

nr_blit_bulk(nr, tileset, config1)
plot(nr)

