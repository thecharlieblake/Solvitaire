#!/bin/bash

# Downloads the image
docker pull thecharlesblake/solvitiare:1.0

# If the cmake-build-[debug/release] folders don't exist, copies pre-generated
# versions from the docker image into the current directory (so new users
# don't have to wait for a long build to complete!)
if [ ! -d "cmake-build-release" && ! -d "cmake-build-debug" ]; then
    docker run -i -v ${PWD}:/.install --rm solvitaire:1.0 \
        bash -c "cp -r ../.install/* ."
fi
