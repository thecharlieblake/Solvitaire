#!/bin/bash

declare -a arr=(
"alpha-star"
"bakers-game"
"black-hole"
"castles-of-spain"
"delta-star"
"eight-off-any-card-spaces"
"eight-off"
"fan"
"flower-garden"
"fore-cell"
"fortunes-favor"
"king-albert"
"northwest-territory"
"one-cell"
"raglan"
"sea-towers"
"somerset"
"spanish-patience"
"spiderette"
"two-cell"
"will-o-the-wisp"
"free-cell-0-cell"
"free-cell-1-cell"
"free-cell-2-cell"
"free-cell-3-cell"
"free-cell-4-pile"
"free-cell-5-pile"
"free-cell-6-pile"
"free-cell-7-pile"
"free-cell"
)

### Presets not attempted:

## Rules incorrect

#"martha"
#"canfield"
#"penguin"

## Too difficult (?)

#"alina"
#"bakers-dozen"
#"scotch-patience"
#"spider"

## Stock games

#"blind-alleys" (medium)
#"chameleon" (hard)
#"duchess" (hard)
#"east-haven" (easy)
#"klondike" (medium)
#"klondike-deal-1" (medium)


DATE=`date +%Y-%m-%d-%H-%M-%S`

mkdir -p results/$DATE

for i in "${arr[@]}"
do
    echo "Running $i ..."
    ./sol --type "$i" --solvability 10000 --time 600000 --cores 25 --stream > results/$DATE/$i.csv
done

echo "Done!"
