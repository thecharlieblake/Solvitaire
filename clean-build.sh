#!/bin/sh

echo "Cleaning..."
rm -r cmake-build-release/*; rm -r cmake-build-debug/*

echo "Installing..."
cd cmake-build-debug; conan install .. --build missing -s compiler=gcc \
	-s compiler.version=6.3 -s compiler.libcxx=libstdc++11; cd ..
cd cmake-build-release; conan install .. --build missing -s compiler=gcc \
	-s compiler.version=6.3 -s compiler.libcxx=libstdc++11; cd ..

echo "Generating build..."
cmake -G "CodeBlocks - Unix Makefiles" -DCMAKE_BUILD_TYPE=DEBUG --target solitaire_pip -Bcmake-build-debug -H. 
cmake -G "CodeBlocks - Unix Makefiles" -DCMAKE_BUILD_TYPE=RELEASE --target solitaire_pip -Bcmake-build-release -H.

echo "Building..."
cmake --build cmake-build-debug
cmake --build cmake-build-release
