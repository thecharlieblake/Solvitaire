# How to build / run the project

The project uses the CMake build system generator. there are several stages for
building the project, running the program, and creating an installer.

### Installing dependencies

#### Upgrade GCC (if you don't have version 6)
`sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y`

`sudo apt-get update -qq`

`sudo apt-get install -qq g++-6`

`sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-6`

#### Boost

Note: this doesn't have to (and probably shouldn't) be run from within the project, unlike the other commands listed here.

`sudo wget -O boost_1_63_0.tar.gz http://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.gz/download`

`sudo tar xzvf boost_1_63_0.tar.gz`

`cd boost_1_63_0/`

`sudo ./bootstrap.sh --with-libraries=log`

`sudo ./b2 link=shared threading=multi toolset=gcc optimization=space`

`sudo ./b2 install`

`cd ..`

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
