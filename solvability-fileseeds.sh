#!/bin/bash


if [ "$#" != 4 ]; then
    echo "Usage: <filename> <cores> \"<solvitaire_command>\" <output_file>"
    echo "e.g.: ./solvability.sh reruns.txt 40 \"sol --type alpha-star --timeout 6000 --str smart-solvability\" results"
    exit 1
fi

seeds="$1"
cores="$2"
sol_command="$3"
minramfree="100G"
out="$4"


echo "Attempted Seed, Outcome, Time Taken(ms), States Searched, Unique States Searched, Backtracks, Dominance Moves, States Removed From Cache, Final States In Cache, Final Buckets In Cache, Maximum Search Depth, Final Search Depth" > "$out.csv"

cat $seeds | parallel --joblog $out.log --progress --memfree $minramfree -j"$cores" "$sol_command" --ra {} --class  >> "$out.csv" 


