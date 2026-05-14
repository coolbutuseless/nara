src="pacman-game.mp4"
dest="pacman-game.gif"
palette="/tmp/palette.png"

ffmpeg -i $src -vf palettegen -y $palette
ffmpeg -i $src -i $palette -lavfi paletteuse -y $dest

gifsicle -o pacman-game2.gif -k 16 --optimize=100  pacman-game.gif
