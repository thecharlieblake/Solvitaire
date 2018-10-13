#!/bin/bash

build="release"
target="solvitaire"
error=false

if [ "$#" == 1 ]; then
    if [ "$1" == "--debug" ]; then
        build="debug"
    elif [ "$1" == "--release" ]; then
        build="release"
    elif [ "$1" == "--unit-tests" ]; then
        target="unit_tests"
    elif [ "$1" == "--solvitaire" ]; then
        target="solvitaire"
    else
        error=true
    fi
fi

if [ "$#" == 2 ]; then
    if [ "$1" == "--debug" ]; then
        build="debug"
    elif [ "$1" == "--release" ]; then
        build="release"
    else
        error=true
    fi

    if [ "$2" == "--unit-tests" ]; then
        target="unit_tests"
    elif [ "$2" == "--solvitaire" ]; then
        target="solvitaire"
    else
        error=true
    fi
fi

if [ "$#" -gt 2 ]; then
    error=true
fi

if [ "$error" = true ]; then
    echo "Usage: ./build.sh [--release|--debug] [--solvitaire|--unit-tests]"
    echo "(default args = --release --solvitaire)"
    exit 1
else
    cmake [-G "CodeBlocks - Unix Makefiles"] \
    "-DCMAKE_BUILD_TYPE=${build^^}" \
    "-Bcmake-build-$build" -H. 
    cmake --build "cmake-build-$build" -- "$target"
    exit 0
fi

