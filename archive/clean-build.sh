#!/bin/sh

echo "Cleaning..."
rm -rf cmake-build-debug; mkdir cmake-build-debug;
rm -rf cmake-build-release; mkdir cmake-build-release;

echo "Generating build..."
cmake -G "CodeBlocks - Unix Makefiles" -D CMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=DEBUG --target solvitaire -Bcmake-build-debug -H. 
cmake -G "CodeBlocks - Unix Makefiles" -D CMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=RELEASE --target solvitaire -Bcmake-build-release -H.

echo "Building..."
cmake --build cmake-build-debug
cmake --build cmake-build-release
