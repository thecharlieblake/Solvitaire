#!/bin/bash

if [ "$#" != 3 ]; then
    # e.g.: ./solvability 1000 40 "sol --type alpha-star --timeout 6000"
    echo "Usage: <seeds> <cores> \"<solvitaire_command>\""
    exit 1
fi

seeds="$1"
cores="$2"
sol_command="$3"

# Features: smart-sol

echo "Attempted Seed, Outcome, Time Taken(ms), States Searched, Unique States Searched, Backtracks, Dominance Moves, States Removed From Cache, Final States In Cache, Final Buckets In Cache, Maximum Search Depth, Final Search Depth"

parallel -j"$cores" "$sol_command" --ra {} --class ::: $(seq "$seeds") 2> /dev/null
