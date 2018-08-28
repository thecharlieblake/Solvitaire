#!/bin/bash

if [ "$#" != 4 ]; then
    echo "Usage: <seeds-filename> <cores> \"<solvitaire_command>\" <output_file>"
    echo "e.g.: ./solvability-fileseeds.sh listOfSeeds.txt 40 \"./sol --type alpha-star --timeout 6000 --str smart-solvability\" results"
    exit 1
fi

seeds="$1"
cores="$2"
sol_command="$3"
out="$4"

time_command="/usr/bin/time -f \" %e, %U, %S, %M\""
combine_command="sed -e 'H;\${x;s/\n/,/g;s/^,//;p;};d'"

minramfree="100G"
completejobs=4

echo "CL: $0 $@" >> "$out.experiment"
echo "Host: $HOSTNAME: " >> "$out.experiment"
echo "StartDate: " `date` >> "$out.experiment"

echo "Attempted Seed, Outcome, Time Taken(ms), States Searched, Unique States Searched, Backtracks, Dominance Moves, States Removed From Cache, Final States In Cache, Final Buckets In Cache, Maximum Search Depth, Final Search Depth, real (time), user (time), sys (time) " >> "$out.experiment"

cat $seeds | parallel --nice 4 --joblog $out.log --progress --memfree $minramfree -j"$cores" "exec $time_command $sol_command --ra {} --class 2>&1 | $combine_command >> $out.csv"
echo "MainRunEndDate: " `date` >> "$out.experiment"

parallel --nice 4 --joblog $out.log --progress --retry-failed -j"$completejobs" 2>&1 

echo "EndDate: " `date` >> "$out.experiment"


