#!/bin/sh

echo "Cleaning..."
rm -r cmake-build-release/*; rm -r cmake-build-debug/*

echo "Installing..."
cd cmake-build-debug; conan install .. --build missing -s compiler=gcc \
	-s compiler.version=6.3 -s compiler.libcxx=libstdc++11; cd ..
cd cmake-build-release; conan install .. --build missing -s compiler=gcc \
	-s compiler.version=6.3 -s compiler.libcxx=libstdc++11; cd ..

echo "Generating build..."
cmake -Bcmake-build-debug -H.
cmake -Bcmake-build-release -H.

echo "Building..."
cmake --build cmake-build-debug
cmake --build cmake-build-release
