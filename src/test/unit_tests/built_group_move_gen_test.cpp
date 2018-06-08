//
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>

#include "../test_helper.h"
#include "../../main/game/search-state/game_state.h"
#include "../../main/game/sol_rules.h"

typedef sol_rules::build_policy pol;

using std::vector;
using std::ostream;
using std::initializer_list;

TEST(BuiltGroupMoveGen, SameSuitEmpty) {
    sol_rules sr;
    sr.tableau_pile_count = 5;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.move_built_group = true;

    test_helper::run_built_group_test(
            sr,
            {
                    {},
                    {"3C","2C","AC"}, // Same suit
                    {"3H","2S","AH"}, // Red black
                    {"3D","2D","AS"}, // Any suit
                    {}
            },
            {
                    // Regular moves
                    move(1, 0, 1),
                    move(2, 0, 1),
                    move(3, 0, 1),
                    move(1, 4, 1),
                    move(2, 4, 1),
                    move(3, 4, 1),

                    // Built pile moves
                    move(1, 0, 2),
                    move(1, 0, 3),
                    move(1, 4, 2),
                    move(1, 4, 3)
            }
    );
}

TEST(BuiltGroupMoveGen, RedBlackEmpty) {
    sol_rules sr;
    sr.tableau_pile_count = 5;
    sr.built_group_pol = pol::RED_BLACK;
    sr.move_built_group = true;

    test_helper::run_built_group_test(
            sr,
            {
                    {},
                    {"3C","2C","AC"}, // Same suit
                    {"3H","2S","AH"}, // Red black
                    {"3D","2D","AS"}, // Any suit
                    {}
            },
            {
                    // Regular moves
                    move(1, 0, 1),
                    move(2, 0, 1),
                    move(3, 0, 1),
                    move(1, 4, 1),
                    move(2, 4, 1),
                    move(3, 4, 1),

                    // Built pile moves
                    move(2, 0, 2),
                    move(2, 0, 3),
                    move(2, 4, 2),
                    move(2, 4, 3),

                    move(3, 0, 2),
                    move(3, 4, 2)
            }
    );
}

TEST(BuiltGroupMoveGen, AnySuitEmpty) {
    sol_rules sr;
    sr.tableau_pile_count = 5;
    sr.built_group_pol = pol::ANY_SUIT;
    sr.move_built_group = true;

    test_helper::run_built_group_test(
            sr,
            {
                    {},
                    {"3C","2C","AC"}, // Same suit
                    {"3H","2S","AH"}, // Red black
                    {"3D","2D","AS"}, // Any suit
                    {}
            },
            {
                    // Regular moves
                    move(1, 0, 1),
                    move(2, 0, 1),
                    move(3, 0, 1),
                    move(1, 4, 1),
                    move(2, 4, 1),
                    move(3, 4, 1),

                    // Built pile moves
                    move(1, 0, 2),
                    move(1, 0, 3),
                    move(1, 4, 2),
                    move(1, 4, 3),

                    move(2, 0, 2),
                    move(2, 0, 3),
                    move(2, 4, 2),
                    move(2, 4, 3),

                    move(3, 0, 2),
                    move(3, 0, 3),
                    move(3, 4, 2),
                    move(3, 4, 3),
            }
    );
}

TEST(BuiltGroupMoveGen, CantMove) {
    sol_rules sr;
    sr.tableau_pile_count = 5;
    sr.built_group_pol = pol::ANY_SUIT;
    sr.move_built_group = false;

    test_helper::run_built_group_test(
            sr,
            {
                    {},
                    {"3C","2C","AC"}, // Same suit
                    {"3H","2S","AH"}, // Red black
                    {"3D","2D","AS"}, // Any suit
                    {}
            },
            {
                    // Regular moves
                    move(1, 0, 1),
                    move(2, 0, 1),
                    move(3, 0, 1),
                    move(1, 4, 1),
                    move(2, 4, 1),
                    move(3, 4, 1)
            }
    );
}

TEST(BuiltGroupMoveGen, SameSuitOccupied) {
    sol_rules sr;
    sr.tableau_pile_count = 5;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.move_built_group = true;

    test_helper::run_built_group_test(
            sr,
            {
                    {"4C"},
                    {"3C","2C","AC"}, // Same suit
                    {"3H","2S","AH"}, // Red black
                    {"3D","2D","AS"}, // Any suit
                    {}
            },
            {
                    // Regular moves
                    move(1, 4, 1),
                    move(2, 4, 1),
                    move(3, 4, 1),
                    move(0, 4, 1),

                    // Built pile moves
                    move(1, 0, 3),
                    move(1, 4, 2),
                    move(1, 4, 3)
            }
    );
}

TEST(BuiltGroupMoveGen, RedBlackOccupied) {
    sol_rules sr;
    sr.tableau_pile_count = 5;
    sr.built_group_pol = pol::RED_BLACK;
    sr.move_built_group = true;

    test_helper::run_built_group_test(
            sr,
            {
                    {"4C"},
                    {"3C","2C","AC"}, // Same suit
                    {"3H","2S","AH"}, // Red black
                    {"3D","2D","AS"}, // Any suit
                    {}
            },
            {
                    // Regular moves
                    move(1, 4, 1),
                    move(2, 4, 1),
                    move(3, 4, 1),
                    move(0, 4, 1),

                    // Built pile moves
                    move(2, 0, 3),
                    move(2, 4, 2),
                    move(2, 4, 3),
                    move(3, 4, 2)
            }
    );
}

TEST(BuiltGroupMoveGen, AnySuitOccupied) {
    sol_rules sr;
    sr.tableau_pile_count = 5;
    sr.built_group_pol = pol::ANY_SUIT;
    sr.move_built_group = true;

    test_helper::run_built_group_test(
            sr,
            {
                    {"4C"},
                    {"3C","2C","AC"}, // Same suit
                    {"3H","2S","AH"}, // Red black
                    {"3D","2D","AS"}, // Any suit
                    {}
            },
            {
                    // Regular moves
                    move(1, 4, 1),
                    move(2, 4, 1),
                    move(3, 4, 1),
                    move(0, 4, 1),

                    // Built pile moves
                    move(1, 0, 3),
                    move(2, 0, 3),
                    move(3, 0, 3),

                    move(1, 4, 2),
                    move(1, 4, 3),

                    move(2, 4, 2),
                    move(2, 4, 3),

                    move(3, 4, 2),
                    move(3, 4, 3),
            }
    );
}
