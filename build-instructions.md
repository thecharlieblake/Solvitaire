# How to build / run the project

The project uses the CMake build system generator. there are several stages for
building the project, running the program, and creating an installer.

## Installing dependencies

`cd cmake-build-debug; conan install ..; cd ..`

## Generate Build System

`cmake -Bcmake-build-debug -H.`

## Build project

`cmake --build cmake-build-debug`

## Run executable

`./cmake-build-debug/bin/solitaire_pip`

## Create project installer

`cpack --config cmake-build-debug/CPackConfig.cmake`

