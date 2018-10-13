#!/bin/bash

trap 'exit 130' INT

if [ "$#" != 4 ]; then
    echo "Usage: <seeds> <cores> <timeout> <cachesize>"
    exit 1
fi

seeds="$1"
cores="$2"
timeout="$3"
cache="$4"

declare -a arr=(
"alpha-star"
"bakers-game"
"black-hole"
"castles-of-spain"
"delta-star"
"eight-off-any-card-spaces"
"eight-off"
"fan"
"fore-cell"
"fortunes-favor"
"king-albert"
"northwest-territory"
"one-cell"
"raglan"
"sea-towers"
"somerset"
"spiderette"
"two-cell"
"will-o-the-wisp"
"free-cell"
"east-haven"
"klondike"
"klondike-deal-1"
"canfield"
"penguin"
"free-cell-0-cell"
"free-cell-1-cell"
"free-cell-2-cell"
"free-cell-3-cell"
"free-cell-4-pile"
"free-cell-5-pile"
"free-cell-6-pile"
"free-cell-7-pile"
"flower-garden"
"spanish-patience"
)


### Presets not attempted:
### Rules wrong?
# "martha"


## Too difficult (?)

# "alina"
# "blind-alleys"
# "bakers-dozen"
# "scotch-patience"
# "chameleon"
# "duchess"
# "spider"


## Stock games



DATE=`date +%Y-%m-%d-%H-%M-%S`

out_dir=results/comparisons-$DATE

mkdir -p "$out_dir"
echo "Writing to $out_dir/"

for i in "${arr[@]}"
do
    echo "Running $i ..."
    ./solvability.sh "$seeds" "$cores" "./cmake-build-release/bin/solvitaire --type $i --timeout $timeout --cache $cache --str none" "$out_dir"/$i-normal
    ./solvability.sh "$seeds" "$cores" "./cmake-build-release/bin/solvitaire-no-auto-foundations --type $i --timeout $timeout --cache $cache --str none" "$out_dir"/$i-no-auto-foundations
    ./solvability.sh "$seeds" "$cores" "./cmake-build-release/bin/solvitaire-no-pile-symmetry --type $i --timeout $timeout --cache $cache --str none" "$out_dir"/$i-no-pile-symmetry
    ./solvability.sh "$seeds" "$cores" "./cmake-build-release/bin/solvitaire-no-suit-symmetry --type $i --timeout $timeout --cache $cache --str none" "$out_dir"/$i-no-suit-symmetry
    ./solvability.sh "$seeds" "$cores" "./cmake-build-release/bin/solvitaire-no-symmetry --type $i --timeout $timeout --cache $cache --str none" "$out_dir"/$i-no-symmetry
done

echo "Done!"
