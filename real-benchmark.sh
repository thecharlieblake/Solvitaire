time (for i in $(seq 1 $2);
do
    ./cmake-build-release/bin/solvitaire --type "$1" --classify --random "$i"
done)
