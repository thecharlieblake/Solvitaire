#!/bin/bash

declare -a arr=(
"somerset"
"alpha-star"
)

DATE=`date +%Y-%m-%d-%H-%M-%S`

mkdir -p results/$DATE

for i in "${arr[@]}"
do
    echo "Running $i ..."
    ./sol --type "$i" --solvability 10000 --time 3600000 --cores 50 --stream > results/$DATE/$i.csv
done

echo "Done!"
