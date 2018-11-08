/*
  Solvitaire: a solver for perfect information solitaire games
  Copyright (C) 2018 Charles Blake <thecharlesblake@live.co.uk> and 
  Ian Gent <Ian.Gent@st-andrews.ac.uk>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program (see LICENSE file); if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
//
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>

#include "../test_helper.h"
#include "../../main/game/search-state/game_state.h"
#include "../../main/game/sol_rules.h"
#include "../../main/game/global_cache.h"

typedef sol_rules::build_policy pol;
typedef sol_rules::face_up_policy fu;
typedef move::mtype mt;
typedef sol_rules::built_group_type bgt;

using std::vector;
using std::ostream;

typedef std::initializer_list<std::initializer_list<std::string>> string_il;

#define ASSERT_FACE_UP(card) ASSERT_FALSE(card.is_face_down()) << card.to_string() << " should be face up"
#define ASSERT_FACE_DOWN(card) ASSERT_TRUE(card.is_face_down()) << card.to_string() << " should be face down"


TEST(FaceUpCards, CapsFaceUpIfFaceUpNotAll) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = bgt::YES;
    sr.face_up = fu::TOP_CARDS;

    string_il piles = {
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
    sr.move_built_group = bgt::YES;
    sr.face_up = fu::ALL;

    string_il piles = {
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
    sr.move_built_group = bgt::YES;
    sr.face_up = fu::TOP_CARDS;

    string_il piles = {
            {"4S"},
            {"3s","AS"}, // from
            {"2S"} // to
    };

    test_helper::expected_moves_test(
            sr,
            piles,
            {
                    // Regular moves
                    move(move::mtype::regular, 1, 2, 1, true)
            }
    );

    game_state gs(sr, piles);
    gs.make_move(move(mt::regular, 1, 2, 1, true));

    ASSERT_FACE_UP(gs.get_data()[0][0]);
    ASSERT_FACE_UP(gs.get_data()[1][0]);
    ASSERT_FACE_UP(gs.get_data()[2][0]);
}

TEST(FaceUpCards, BuiltGroupMoveTurnsFaceUp) {
    sol_rules sr;
    sr.tableau_pile_count = 2;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = bgt::YES;
    sr.face_up = fu::TOP_CARDS;

    string_il piles = {
            {"3s","2S","AS"}, // from
            {} // to
    };

    test_helper::expected_moves_test(
            sr,
            piles,
            {
                    // Regular moves
                    move(move::mtype::regular, 0, 1, 1, false),
                    // Built-group moves
                    move(move::mtype::built_group, 0, 1, 2, true)
            }
    );

    game_state gs(sr, piles);
    gs.make_move(move(mt::built_group, 0, 1, 2, true));

    ASSERT_FACE_UP(gs.get_data()[0][0]);
    ASSERT_FACE_UP(gs.get_data()[1][0]);
    ASSERT_FACE_UP(gs.get_data()[1][1]);
}

TEST(FaceUpCards, MoveAlreadyFaceUpNoChange) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = bgt::YES;
    sr.face_up = fu::TOP_CARDS;

    string_il piles = {
            {"4S"},
            {"3s","AS"}, // from
            {"2S"} // to
    };

    test_helper::expected_moves_test(
            sr,
            piles,
            {
                    // Regular moves
                    move(move::mtype::regular, 1, 2, 1, true)
            }
    );

    game_state gs(sr, piles);
    gs.make_move(move(mt::regular, 1, 2, 1, true));

    ASSERT_FACE_UP(gs.get_data()[0][0]);
    ASSERT_FACE_UP(gs.get_data()[1][0]);
    ASSERT_FACE_UP(gs.get_data()[2][0]);
}

TEST(FaceUpCards, FaceDownNoBuiltGroup) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = bgt::YES;
    sr.face_up = fu::TOP_CARDS;

    test_helper::expected_moves_test(
            sr,
            {
                    {"4S"},
                    {"3s","AS"},
                    {"2S"}
            },
            {
                    // Regular moves
                    move(move::mtype::regular, 1, 2, 1, true)
            }
    );
}

TEST(FaceUpCards, FaceUpBuiltGroup) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = bgt::YES;
    sr.face_up = fu::TOP_CARDS;

    test_helper::expected_moves_test(
            sr,
            {
                    {"3S"},
                    {"2S","AS"},
                    {}
            },
            {
                    // Regular moves
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
    sr.move_built_group = bgt::YES;
    sr.face_up = fu::TOP_CARDS;

    test_helper::expected_moves_test(
            sr,
            {
                    {"4S"},
                    {"3s","2S","AS"},
                    {"3S"}
            },
            {
                    // Regular moves
                    move(mt::regular, 2, 0, 1, false),

                    // Built group moves
                    move(mt::built_group, 1, 2, 2, true)
            }
    );
}

TEST(FaceUpCards, UndoTurnsBackFaceDown) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::SAME_SUIT;
    sr.build_pol = pol::SAME_SUIT;
    sr.move_built_group = bgt::YES;
    sr.face_up = fu::TOP_CARDS;

    string_il piles = {
            {"3S"},
            {"2s","AS"}, // from
            {} // to
    };

    game_state gs(sr, piles);
    move m = move(mt::regular, 1, 2, 1, true);
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
    sr.move_built_group = bgt::YES;
    sr.face_up = fu::TOP_CARDS;

    string_il piles = {
            {"4S"},
            {"3s","2S","AS"}, // from
            {} // to
    };

    game_state gs(sr, piles);
    move m = move(mt::built_group, 1, 2, 2, true);
    gs.make_move(m);
    gs.undo_move(m);

    ASSERT_FACE_UP(gs.get_data()[0][0]);
    ASSERT_FACE_UP(gs.get_data()[1][0]);
    ASSERT_FACE_UP(gs.get_data()[1][1]);
    ASSERT_FACE_DOWN(gs.get_data()[1][2]);
}

TEST(FaceUpCards, CacheEncodesFaceDown) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.built_group_pol = pol::RED_BLACK;
    sr.build_pol = pol::RED_BLACK;
    sr.move_built_group = bgt::YES;
    sr.face_up = fu::TOP_CARDS;

    string_il piles = {
            {"2C"},
            {"2s","AH"},
            {}
    };

    game_state gs(sr, piles);
    lru_cache cache(gs, 1000);
    bool new_state;

    cache.insert(gs);

    gs.make_move(move(mt::regular, 1, 2, 1, true));
    new_state = cache.insert(gs).second;
    ASSERT_TRUE(new_state) << "AH to empty pile";

    gs.make_move(move(mt::regular, 2, 1, 1, false));
    new_state = cache.insert(gs).second;
    ASSERT_TRUE(new_state) << "AH back to face-up 2S";

    gs.make_move(move(mt::regular, 1, 0, 1, false));
    new_state = cache.insert(gs).second;
    ASSERT_TRUE(new_state) << "AH to 2C";

    gs.make_move(move(mt::regular, 0, 1, 1, false));
    new_state = cache.insert(gs).second;
    ASSERT_FALSE(new_state) << "AH to 2S";
}
