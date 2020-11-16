#!/bin/bash
for i in $(seq "$1")
do
  seed=`shuf -i 1-999999999 -n 1`
  final_search="`./solvitaire --type klondike --random $seed | grep "Final Search"`"
  echo "Seed-Number:$seed $final_search"
done
