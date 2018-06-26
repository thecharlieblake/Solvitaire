//
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>

#include "../test_helper.h"
#include "../../main/game/search-state/game_state.h"
#include "../../main/game/sol_rules.h"

typedef sol_rules::build_policy pol;
typedef sol_rules::face_up_policy fu;
typedef move::mtype mt;

using std::vector;
using std::ostream;
using std::initializer_list;

#define ASSERT_FACE_UP(card) ASSERT_TRUE(card.is_face_up()) << card.to_string() << " should be face up"
#define ASSERT_FACE_DOWN(card) ASSERT_FALSE(card.is_face_up()) << card.to_string() << " should be face down"


TEST(FaceUpCards, CapsFaceUpIfFaceUpNotAll) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = true;
    sr.face_up = fu::TOP_CARDS;

    initializer_list<pile> piles = {
            {"3S"},
            {"2s","AS"},
            {}
    };

    game_state gs(sr, piles);

    ASSERT_FACE_UP(gs.get_data()[0][0]);
    ASSERT_FACE_UP(gs.get_data()[1][0]);
    ASSERT_FACE_DOWN(gs.get_data()[1][1]);
}

TEST(FaceUpCards, IgnoreCapsIfFaceUpAll) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = true;
    sr.face_up = fu::ALL;

    initializer_list<pile> piles = {
            {"3S"},
            {"2s","AS"},
            {}
    };

    game_state gs(sr, piles);

    ASSERT_FACE_UP(gs.get_data()[0][0]);
    ASSERT_FACE_UP(gs.get_data()[1][0]);
    ASSERT_FACE_UP(gs.get_data()[1][1]);
}

TEST(FaceUpCards, MoveTurnsFaceUp) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = true;
    sr.face_up = fu::TOP_CARDS;

    initializer_list<pile> piles = {
            {"3S"},
            {"2s","AS"}, // from
            {} // to
    };

    game_state gs(sr, piles);
    gs.make_move(move(mt::regular, 1, 2));

    ASSERT_FACE_UP(gs.get_data()[0][0]);
    ASSERT_FACE_UP(gs.get_data()[1][0]);
    ASSERT_FACE_UP(gs.get_data()[2][0]);
}

TEST(FaceUpCards, BuiltGroupMoveTurnsFaceUp) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = true;
    sr.face_up = fu::TOP_CARDS;

    initializer_list<pile> piles = {
            {"4S"},
            {"3s","2S","AS"}, // from
            {} // to
    };

    game_state gs(sr, piles);
    gs.make_move(move(mt::built_group, 1, 2, 2));

    ASSERT_FACE_UP(gs.get_data()[0][0]);
    ASSERT_FACE_UP(gs.get_data()[1][0]);
    ASSERT_FACE_UP(gs.get_data()[2][0]);
    ASSERT_FACE_UP(gs.get_data()[2][1]);
}

TEST(FaceUpCards, MoveAlreadyFaceUpNoChange) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = true;
    sr.face_up = fu::TOP_CARDS;

    initializer_list<pile> piles = {
            {"3S"},
            {"2S","AS"}, // from
            {} // to
    };

    game_state gs(sr, piles);
    gs.make_move(move(mt::regular, 1, 2));

    ASSERT_FACE_UP(gs.get_data()[0][0]);
    ASSERT_FACE_UP(gs.get_data()[1][0]);
    ASSERT_FACE_UP(gs.get_data()[2][0]);
}

TEST(FaceUpCards, FaceDownNoBuiltGroup) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = true;
    sr.face_up = fu::TOP_CARDS;

    initializer_list<pile> piles = {
            {"3S"},
            {"2s","AS"},
            {}
    };

    test_helper::expected_moves_test(
            sr,
            {
                    {"3S"},
                    {"2s","AS"},
                    {}
            },
            {
                    // Regular moves
                    move(move::mtype::regular, 0, 2, 1),
                    move(move::mtype::regular, 1, 2, 1),
            }
    );
}

TEST(FaceUpCards, FaceUpBuiltGroup) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = true;
    sr.face_up = fu::TOP_CARDS;

    initializer_list<pile> piles = {
            {"3S"},
            {"2s","AS"},
            {}
    };

    test_helper::expected_moves_test(
            sr,
            {
                    {"3S"},
                    {"2S","AS"},
                    {}
            },
            {
                    // Regular moves
                    move(mt::regular, 0, 2, 1),
                    move(mt::regular, 1, 2, 1),

                    // Built group moves
                    move(mt::built_group, 1, 0, 2),
                    move(mt::built_group, 1, 2, 2)
            }
    );
}

TEST(FaceUpCards, FaceUpBuiltGroup2Not3) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = true;
    sr.face_up = fu::TOP_CARDS;

    initializer_list<pile> piles = {
            {"3S"},
            {"2s","AS"},
            {}
    };

    test_helper::expected_moves_test(
            sr,
            {
                    {"4S"},
                    {"3s","2S","AS"},
                    {}
            },
            {
                    // Regular moves
                    move(mt::regular, 0, 2, 1),
                    move(mt::regular, 1, 2, 1),

                    // Built group moves
                    move(mt::built_group, 1, 2, 2)
            }
    );
}

TEST(FaceUpCards, UndoTurnsBackFaceDown) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = true;
    sr.face_up = fu::TOP_CARDS;

    initializer_list<pile> piles = {
            {"3S"},
            {"2s","AS"}, // from
            {} // to
    };

    game_state gs(sr, piles);
    move m = move(mt::regular, 1, 2);
    gs.make_move(m);
    gs.undo_move(m);

    ASSERT_FACE_UP(gs.get_data()[0][0]);
    ASSERT_FACE_UP(gs.get_data()[1][0]);
    ASSERT_FACE_DOWN(gs.get_data()[1][1]);
}

TEST(FaceUpCards, UndoBuiltGroupTurnsBackFaceDown) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = true;
    sr.face_up = fu::TOP_CARDS;

    initializer_list<pile> piles = {
            {"4S"},
            {"3s","2S","AS"}, // from
            {} // to
    };

    game_state gs(sr, piles);
    move m = move(mt::built_group, 1, 2, 2);
    gs.make_move(m);
    gs.undo_move(m);

    ASSERT_FACE_UP(gs.get_data()[0][0]);
    ASSERT_FACE_UP(gs.get_data()[1][0]);
    ASSERT_FACE_UP(gs.get_data()[1][1]);
    ASSERT_FACE_DOWN(gs.get_data()[1][2]);
}
