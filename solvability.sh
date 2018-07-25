#!/bin/bash

if [ "$#" != 4 ]; then
    echo "Usage: <seeds> <cores> \"<solvitaire_command>\" <output_file>"
    echo "e.g.: ./solvability.sh 1000 40 \"sol --type alpha-star --timeout 6000\" results.csv"
    exit 1
fi

seeds="$1"
cores="$2"
sol_command="$3"
out="$4"

echo "Attempted Seed, Outcome, Time Taken(ms), States Searched, Unique States Searched, Backtracks, Dominance Moves, States Removed From Cache, Final States In Cache, Final Buckets In Cache, Maximum Search Depth, Final Search Depth" > "$out"

parallel -j"$cores" "$sol_command" --str smart-solvability --ra {} --class ::: $(seq "$seeds") >> "$out" 2> /dev/null
