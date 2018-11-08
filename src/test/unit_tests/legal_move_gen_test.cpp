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
#include "../../main/game/move.h"

typedef sol_rules::build_policy pol;
typedef sol_rules::spaces_policy s_pol;
typedef sol_rules::stock_deal_type sdt;
typedef sol_rules::built_group_type bgt;

using std::vector;
using std::ostream;

typedef std::initializer_list<std::initializer_list<std::string>> string_il;


TEST(LegalMoveGen, BuildPolAnySuit) {
    sol_rules sr;
    sr.build_pol = pol::ANY_SUIT;
    sr.tableau_pile_count = 5;

    game_state gs(sr, string_il{
            {"2C"},
            {"2H"},
            {"2D"},
            {"2S"},
            {"AC"}
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
            move(move::mtype::regular, 4,0),
            move(move::mtype::regular, 4,1),
            move(move::mtype::regular, 4,2),
            move(move::mtype::regular, 4,3)
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, BuildPolRedBlack) {
    sol_rules sr;
    sr.build_pol = pol::RED_BLACK;
    sr.tableau_pile_count = 5;

    game_state gs(sr, string_il{
            {"2C"},
            {"2H"},
            {"2S"},
            {"2D"},
            {"AC"}
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
            move(move::mtype::regular, 4,1),
            move(move::mtype::regular, 4,3)
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, BuildPolSameSuit) {
    sol_rules sr;
    sr.build_pol = pol::SAME_SUIT;
    sr.tableau_pile_count = 5;

    game_state gs(sr, string_il{
            {"2C"},
            {"2H"},
            {"2S"},
            {"2D"},
            {"AC"}
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
            move(move::mtype::regular, 4,0)
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, BuildPolNoBuild) {
    sol_rules sr;
    sr.build_pol = pol::NO_BUILD;
    sr.tableau_pile_count = 5;

    game_state gs(sr, string_il{
            {"2C"},
            {"2H"},
            {"2S"},
            {"2D"},
            {"AC"}
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, SpacesPolAny) {
    sol_rules sr;
    sr.spaces_pol = s_pol::ANY;
    sr.tableau_pile_count = 2;

    game_state gs(sr, string_il{
            {},
            {"AC"}
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
    //        move(move::mtype::regular, 1, 0) // Now forbidden as always pointless
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, SpacesPolKings) {
    sol_rules sr;
    sr.spaces_pol = s_pol::KINGS;
    sr.tableau_pile_count = 3;

    game_state gs(sr, string_il{
            {},
            {"AC"},
            {"3S","KD"}
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
            move(move::mtype::regular, 2, 0)
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, SpacesPolNoBuild) {
    sol_rules sr;
    sr.spaces_pol = s_pol::NO_BUILD;
    sr.tableau_pile_count = 2;

    game_state gs(sr, string_il{
            {},
            {"AC"}
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, MoveBuiltGroupTrue) {
    sol_rules sr;
    sr.move_built_group = bgt::YES;
    sr.tableau_pile_count = 2;

    game_state gs(sr, string_il{
            {},
            {"2C", "AC"}
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
            move(move::mtype::regular, 1, 0, 1),
            move(move::mtype::built_group, 1, 0, 2)
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, MoveBuiltGroupFalse) {
    sol_rules sr;
    sr.move_built_group = bgt::NO;
    sr.tableau_pile_count = 2;

    game_state gs(sr, string_il{
            {},
            {"AC", "2C"}
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
            move(move::mtype::regular, 1, 0, 1)
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, Foundations) {
    sol_rules sr;
    sr.foundations_present = true;
    sr.tableau_pile_count = 4;

    game_state gs(sr, string_il{
            {}, {}, {}, {},
            {"AC"},
            {"AH"},
            {"AS"},
            {"AD"},
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
            move(move::mtype::regular, 4, 0),
            move(move::mtype::regular, 5, 1),
            move(move::mtype::regular, 6, 2),
            move(move::mtype::regular, 7, 3)
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, FoundationsRemovable) {
    sol_rules sr;
    sr.foundations_present = true;
    sr.foundations_removable = true;
    sr.tableau_pile_count = 1;

    game_state gs(sr, string_il{
            {"AC","2C","3C"}, // Without a few cards here dominance logic blocks
            {},               // removing from foundation
            {},
            {},
            {}
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
            move(move::mtype::regular, 0, 4)
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}


TEST(LegalMoveGen, TwoDecksFoundations) {
    sol_rules sr;
    sr.two_decks = true;
    sr.foundations_removable = true;
    sr.foundations_present = true;
    sr.tableau_pile_count = 4;

    game_state gs(sr, string_il{
            {"AC"}, {}, {}, {}, {}, {}, {"AS"}, {},
            {},
            {"AH"},
            {},
            {"AD"},
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
            // Aces up
            move(move::mtype::regular, 9, 1),
            move(move::mtype::regular, 9, 5),
            move(move::mtype::regular, 11, 3),
            move(move::mtype::regular, 11, 7),

            // Aces down
            move(move::mtype::regular, 0, 8),
            move(move::mtype::regular, 0, 10),
            move(move::mtype::regular, 6, 8),
            move(move::mtype::regular, 6, 10),

            // Aces across (forbidden now)
            //move(move::mtype::regular, 9, 8),
            //move(move::mtype::regular, 9, 10),
            //move(move::mtype::regular, 11, 8),
            //move(move::mtype::regular, 11, 10),

    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, Cells) {
    sol_rules sr;
    sr.cells = 2;
    sr.tableau_pile_count = 1;

    game_state gs(sr, string_il{
            {"3D"}, {}, // The cells
            {"4H"}
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
            move(move::mtype::regular, 0, 2),
            move(move::mtype::regular, 2, 1)
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, StockDealToTableau) {
    sol_rules sr;
    sr.foundations_present = true;
    sr.tableau_pile_count = 3;
    sr.stock_size = 2;
    sr.stock_deal_t = sdt::TABLEAU_PILES;

    game_state gs(sr, string_il{
            {}, {}, {}, {}, // The foundations
            {"AC", "AD"}, // The stock
            {"3H"}, {"5D"}, {"7C"} // The tableau piles
    });
    vector<move> actual_moves = gs.get_legal_moves();

    vector<move> exp_moves = {
            move(move::mtype::stock_to_all_tableau, 0, 0, 2) // Special stock-to-tableau syntax
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}
