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
"blind-alleys"
"chameleon"
"duchess"
"east-haven"
"klondike"
"klondike-deal-1"
"martha"
"canfield"
"penguin"
"alina"
"bakers-dozen"
"scotch-patience"
"spider"
)

### Presets not attempted:

## Rules incorrect


## Too difficult (?)


## Stock games



DATE=`date +%Y-%m-%d-%H-%M-%S`

mkdir -p results/$DATE

for i in "${arr[@]}"
do
    echo "Running $i ..."
    sol --type "$i" --solvability 20 --time 60 --stream none > results/$DATE/$i.csv
done

echo "Done!"
