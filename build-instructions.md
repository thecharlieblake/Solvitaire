# How to build / run the project

The project uses the CMake build system generator. there are several stages for
building the project, running the program, and creating an installer.

### Installing dependencies

`cd cmake-build-[debug|release]; conan install .. --build missing -s \
compiler=gcc -s compiler.version=6.3 -s compiler.libcxx=libstdc++11; cd ..`

### Generate Build System

`cmake [-G "CodeBlocks - Unix Makefiles"] -D CMAKE_CXX_COMPILER=g++-6 -DCMAKE_BUILD_TYPE=[DEBUG|RELEASE] -Bcmake-build-[debug|release] -H.`

### Build project

`cmake --build cmake-build-[debug|release]`

### Run executable

`./cmake-build-[debug|release]/bin/solvitaire`

### Create project installer

`cpack --config cmake-build-[debug|release]/CPackConfig.cmake`

## A Note on CLion

If CLion complains that a dependency is missing, you may need to run the conan
install command, and then in Clion run 'File->Reload CMake Project
