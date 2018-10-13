time (for i in $(seq 1 $3);
do
    ./cmake-build-release/bin/solvitaire --type "$1" --classify --random "$2" > /dev/null 2>&1
done)
