#!/bin/bash

trap 'exit 130' INT

if [ "$#" != 3 ]; then
    echo "Usage: <seeds> <cores> <timeout>"
    exit 1
fi

seeds="$1"
cores="$2"
timeout="$3"

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

#"martha"
#"penguin"

## Too difficult (?)


## Stock games



DATE=`date +%Y-%m-%d-%H-%M-%S`

out_dir=results/$DATE

mkdir -p "$out_dir"
echo "Writing to $out_dir/"

for i in "${arr[@]}"
do
    echo "Running $i ..."
    ./solvability.sh "$seeds" "$cores" "./cmake-build-release/bin/solvitaire --type $i --timeout $timeout" "$out_dir"/$i.csv
done

echo "Done!"
