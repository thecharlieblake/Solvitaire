# File based on solitaire_pip at https://stackoverflow.com/questions/21163188/most-simple-but-complete-cmake-example

cmake_minimum_required(VERSION 3.0)
project(solitaire_pip)

set(CMAKE_CXX_STANDARD 14)

###############################################################################
## source files ###############################################################
###############################################################################

set(sources src/main/main.cpp)
set(sources_test src/test/all_tests.cpp src/test/main_test.cpp)

###############################################################################
## conan.io setup #############################################################
###############################################################################

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    # Disable warnings/errors for conan libraries
    set(CONAN_SYSTEM_INCLUDES On)
    conan_basic_setup()
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

###############################################################################
## compile options ############################################################
###############################################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pedantic -Wall -Wextra \
-Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wformat=2 -Winit-self \
-Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept \
-Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow \
-Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s -DNDEBUG -Wl,-O1")

###############################################################################
## target definitions #########################################################
###############################################################################

# Default to building the 'Debug' configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "The build configuration" FORCE)
endif()

# add the data to the target, so it becomes visible in some IDE
add_executable(solitaire_pip ${sources})

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