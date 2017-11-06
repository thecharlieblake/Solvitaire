# File based on https://stackoverflow.com/questions/21163188/most-simple-but-complete-cmake-example

cmake_minimum_required(VERSION 3.0)
project(solvitaire)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

###############################################################################
## source files ###############################################################
###############################################################################

set(sources src/main/main.cpp src/main/command_line_helper.cpp
        src/main/command_line_helper.h src/main/log_helper.h src/main/card.h
        src/main/card.cpp src/main/deal_parser.cpp src/main/deal_parser.h
        src/main/solver.cpp src/main/solver.h src/main/game_state.cpp
        src/main/game_state.h src/main/sol_rules.h src/main/sol_rules.cpp
        src/main/pile.cpp src/main/pile.h)
set(sources_test src/test/main_test.cpp)

###############################################################################
## Rapid JSON setup ###########################################################
###############################################################################

find_package(RapidJSON)

###############################################################################
## Boost setup ################################################################
###############################################################################

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(boost_components log program_options)

find_package(Boost 1.63.0 REQUIRED COMPONENTS ${boost_components})

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(WARNING "Must install Boost libraries")
endif()

###############################################################################
## GTest setup ################################################################
###############################################################################

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()

###############################################################################
## compile options ############################################################
###############################################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -DBOOST_LOG_DYN_LINK -pthread -pedantic -Wall -Wextra \
-Wcast-align -Wcast-qual -Wformat=2 -Winit-self \
-Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept \
-Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow \
-Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Werror")
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
add_executable(solvitaire ${sources})

# this lets me include files relative to the root src dir with a <> pair
target_include_directories(solvitaire PUBLIC src/main)

target_link_libraries(solvitaire LINK_PUBLIC ${Boost_LIBRARIES})

###############################################################################
## testing ####################################################################
###############################################################################

add_executable(unit_tests ${sources_test})

# this allows us to use our executable as a link library
# therefore we can inherit all compiler options and library dependencies
set_target_properties(solvitaire PROPERTIES ENABLE_EXPORTS on)

target_link_libraries(unit_tests gtest_main)

ENABLE_TESTING()
ADD_TEST(NAME unit_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        COMMAND unit_tests)

###############################################################################
## packaging ##################################################################
###############################################################################

# all install commands get the same destination. this allows us to use paths
# relative to the executable.
install(TARGETS solvitaire DESTINATION solvitaire)

set(CPACK_OUTPUT_FILE_PREFIX installer)
set(CPACK_PACKAGE_VERSION "1.0.0")

# we don't want to split our program up into several things
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)