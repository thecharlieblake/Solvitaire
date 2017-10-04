# File based on solitaire_pip at https://stackoverflow.com/questions/21163188/most-simple-but-complete-cmake-example

cmake_minimum_required(VERSION 3.0)
project(solitaire_pip)

set(CMAKE_CXX_STANDARD 14)

###############################################################################
## file globbing ##############################################################
###############################################################################

# these instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables 
# `sources` and `data`
set(sources src/main/main.cpp)
set(sources_test src/test/all_tests.cpp src/test/main_test.cpp)

###############################################################################
## conan.io setup #############################################################
###############################################################################

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

###############################################################################
## target definitions #########################################################
###############################################################################

# add the data to the target, so it becomes visible in some IDE
add_executable(solitaire_pip ${sources})

# just for example add some compiler flags
target_compile_options(solitaire_pip PUBLIC -std=c++1y -Wall -Wfloat-conversion)

# this lets me include files relative to the root src dir with a <> pair
target_include_directories(solitaire_pip PUBLIC src/main)

# link conan libraries
target_link_libraries(solitaire_pip ${CONAN_LIBS})

# this copies all resource files in the build directory
# we need this, because we want to work with paths relative to the executable
file(COPY ${data} DESTINATION resources)

###############################################################################
## testing ####################################################################
###############################################################################

add_executable(unit_tests ${sources_test})

# this allows us to use our executable as a link library
# therefore we can inherit all compiler options and library dependencies
set_target_properties(solitaire_pip PROPERTIES ENABLE_EXPORTS on)

target_link_libraries(unit_tests ${CONAN_LIBS} solitaire_pip)

ENABLE_TESTING()
ADD_TEST(NAME unit_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        COMMAND unit_tests)

###############################################################################
## packaging ##################################################################
###############################################################################

# all install commands get the same destination. this allows us to use paths
# relative to the executable.
install(TARGETS solitaire_pip DESTINATION solitaire_pip)
# this is basically a repeat of the file copy instruction that copies the
# resources in the build directory, but here we tell cmake that we want it
# in the package
install(DIRECTORY resources DESTINATION solitaire_pip)

set(CPACK_OUTPUT_FILE_PREFIX installer)
set(CPACK_PACKAGE_VERSION "1.0.0")

# we don't want to split our program up into several things
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)