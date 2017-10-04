#!/bin/sh

echo "Cleaning..."
rm -r cmake-build-release/*; rm -r cmake-build-debug/*

echo "Installing..."
cd cmake-build-debug; conan install ..; cd ..
cd cmake-build-release; conan install ..; cd ..

echo "Generating build..."
cmake -Bcmake-build-debug -H.
cmake -Bcmake-build-release -H.

echo "Building..."
cmake --build cmake-build-debug
cmake --build cmake-build-release
