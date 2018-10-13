#!/bin/bash

if [ "$#" == 0 ]; then
    docker run -it -v$(pwd):/home/Solvitaire --rm \
    thecharlesblake/solvitaire:1.0
else
    docker run -it -v$(pwd):/home/Solvitaire --rm \
    thecharlesblake/solvitaire:1.0 bash -c "$*"
fi
