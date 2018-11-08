# File based on https://stackoverflow.com/questions/21163188/most-simple-but-complete-cmake-example

cmake_minimum_required(VERSION 3.9.4)
project(solvitaire)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# For -flto flag
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

###############################################################################
## source files ###############################################################
###############################################################################

set(main src/main/main.cpp)
set(sources src/main/input-output/input/command_line_helper.cpp
        src/main/input-output/input/command_line_helper.h
        src/main/input-output/output/log_helper.h
        src/main/game/card.h
        src/main/game/card.cpp
        src/main/input-output/input/json-parsing/deal_parser.h
        src/main/input-output/input/json-parsing/deal_parser.cpp
        src/main/solver/solver.cpp
        src/main/solver/solver.h
        src/main/game/search-state/game_state.cpp
        src/main/game/search-state/game_state.h
        src/main/game/sol_rules.h
        src/main/game/pile.cpp
        src/main/game/pile.h
        src/main/input-output/input/json-parsing/json_helper.cpp
        src/main/input-output/input/json-parsing/json_helper.h
        src/main/input-output/input/sol_preset_types.h
        src/main/input-output/input/sol_preset_types.cpp
        src/main/input-output/input/json-parsing/rules_parser.cpp
        src/main/input-output/input/json-parsing/rules_parser.h
        src/main/input-output/output/state_printer.cpp
        src/main/input-output/output/state_printer.h
        src/main/game/global_cache.cpp
        src/main/game/global_cache.h
        src/main/game/sol_rules.cpp
        src/main/evaluation/solvability_calc.cpp
        src/main/evaluation/solvability_calc.h
        src/main/evaluation/benchmark.cpp
        src/main/evaluation/benchmark.h
		lib/rapidjson/document.h
		lib/rapidjson/schema.h
		lib/rapidjson/stringbuffer.h
        src/main/game/search-state/game_state.legal_moves.cpp
        src/main/game/search-state/game_state.dominance_moves.cpp
        src/main/game/search-state/game_state.pile_order.cpp
        src/main/game/move.cpp
        src/main/game/move.h src/main/evaluation/binomial_ci.cpp src/main/evaluation/binomial_ci.h)
set(sources_test
        src/test/test_helper.cpp
        src/test/test_helper.h
        src/test/integration_tests/spanish_patience_test.cpp
        src/test/integration_tests/black_hole_test.cpp
        src/test/integration_tests/free_cell_test.cpp
        src/test/integration_tests/bakers_dozen_test.cpp
        src/test/integration_tests/fortunes_favor_test.cpp
        src/test/integration_tests/flower_garden_test.cpp
        src/test/integration_tests/canfield_test.cpp
        src/test/integration_tests/somerset_test.cpp
        src/test/integration_tests/alpha_star_test.cpp
        src/test/integration_tests/spider_test.cpp
        src/test/integration_tests/klondike_test.cpp
        src/test/integration_tests/gaps_test.cpp
        src/test/integration_tests/accordion_test.cpp
        src/test/unit_tests/global_cache_test.cpp
        src/test/unit_tests/foundations_dominance_test.cpp
        src/test/unit_tests/deal_parser_test.cpp
        src/test/unit_tests/card_test.cpp
        src/test/unit_tests/pile_test.cpp
        src/test/unit_tests/legal_move_gen_test.cpp
        src/test/unit_tests/built_group_move_gen_test.cpp
        src/test/unit_tests/face_up_cards_test.cpp
        src/test/unit_tests/k_plus_stock_test.cpp)

###############################################################################
## Rapid JSON setup ###########################################################
###############################################################################

include_directories(lib/rapidjson)

###############################################################################
## Boost setup ################################################################
###############################################################################

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(boost_components program_options)

find_package(Boost 1.53.0 REQUIRED COMPONENTS ${boost_components})

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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -DBOOST_LOG_DYN_LINK \
-lboost_system -pthread -pedantic -Wall -Wextra \
-Wcast-align -Wcast-qual -Wformat=2 -Winit-self \
-Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept \
-Wno-old-style-cast -Woverloaded-virtual -Wredundant-decls -Wno-shadow \
-Wstrict-null-sentinel -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O -fno-inline -fno-inline-functions -g -Wl,-O1") # uncomment for profiling
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto -s -DNDEBUG -Wl,-O1") # uncomment otherwise

###############################################################################
## target definitions #########################################################
###############################################################################

# Default to building the 'Debug' configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "The build configuration" FORCE)
endif()

# add the data to the target, so it becomes visible in some IDE
add_executable(solvitaire ${main} ${sources})
set_property(TARGET solvitaire PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE) # For -flto flag
target_link_libraries(solvitaire LINK_PUBLIC ${Boost_LIBRARIES})

# add a separete executable for the 'no pile symmetry' version of the solver
#add_executable(solvitaire-no-pile-symmetry ${main} ${sources})
#target_link_libraries(solvitaire-no-pile-symmetry LINK_PUBLIC ${Boost_LIBRARIES})
#set_target_properties(solvitaire-no-pile-symmetry PROPERTIES COMPILE_FLAGS -DNO_PILE_SYMMETRY)

# add a separete executable for the 'no suit symmetry' version of the solver
#add_executable(solvitaire-no-suit-symmetry ${main} ${sources})
#target_link_libraries(solvitaire-no-suit-symmetry LINK_PUBLIC ${Boost_LIBRARIES})
#set_target_properties(solvitaire-no-suit-symmetry PROPERTIES COMPILE_FLAGS -DNO_SUIT_SYMMETRY)

# add a separete executable for the 'no symmetry' version of the solver
#add_executable(solvitaire-no-symmetry ${main} ${sources})
#target_link_libraries(solvitaire-no-symmetry LINK_PUBLIC ${Boost_LIBRARIES})
#set_target_properties(solvitaire-no-symmetry PROPERTIES COMPILE_FLAGS "-DNO_SUIT_SYMMETRY -DNO_PILE_SYMMETRY")

# add a separete executable for the 'no auto foundations' version of the solver
#add_executable(solvitaire-no-auto-foundations ${main} ${sources})
#target_link_libraries(solvitaire-no-auto-foundations LINK_PUBLIC ${Boost_LIBRARIES})
#set_target_properties(solvitaire-no-auto-foundations PROPERTIES COMPILE_FLAGS -DNO_AUTO_FOUNDATIONS)


###############################################################################
## testing ####################################################################
###############################################################################

add_executable(unit_tests ${sources_test} ${sources})

# this allows us to use our executable as a link library
# therefore we can inherit all compiler options and library dependencies
set_target_properties(solvitaire PROPERTIES ENABLE_EXPORTS on)

target_link_libraries(unit_tests gtest_main ${Boost_LIBRARIES})

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
