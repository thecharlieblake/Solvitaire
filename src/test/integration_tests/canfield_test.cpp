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
// Created by thecharlesblake on 10/4/17.
//

#include <gtest/gtest.h>

#include "../test_helper.h"
#include "../../main/input-output/input/json-parsing/rules_parser.h"
#include "../../main/game/search-state/game_state.h"

using std::vector;

typedef test_helper th;
typedef game_state::streamliner_options so;
typedef std::initializer_list<std::initializer_list<std::string>> string_il;

const static std::string path = "resources/canfield/";
const static std::string type = "-test-canfield";

TEST(Canfield, SimpleSolvable) {
    EXPECT_TRUE(th::is_solvable(path + "SimpleSolvable.json", type));
}

TEST(Canfield, ComplexSolvable) {
    EXPECT_TRUE(th::is_solvable(path + "ComplexSolvable.json", type));
}

TEST(Canfield, SimpleUnsolvable) {
    EXPECT_FALSE(th::is_solvable(path + "SimpleUnsolvable.json", type));
}

TEST(Canfield, ComplexUnsolvable) {
    EXPECT_FALSE(th::is_solvable(path + "ComplexUnsolvable.json", type));
}

TEST(Canfield, FoundationSingleCard) {
    sol_rules sr = rules_parser::from_preset("canfield");
    game_state gs(sr, 0, so::NONE);

    ASSERT_EQ(1, test_helper::cards_in_founds(gs));
}

TEST(Canfield, FoundationWraps) {
    sol_rules sr = rules_parser::from_preset("-test-canfield");

    game_state gs(sr, string_il{
            {"3C"},{},{},{}, // Foundations
            {},{}, // Stock-waste
            {}, // Reserve
            {"AC"},{},{} // Tableau piles
    });

    vector<move> actual_moves = gs.get_legal_moves();
    vector<move> exp_moves = {
            move(move::mtype::regular, 7, 0, 1)
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(Canfield, MoveWholePile) {
    sol_rules sr = rules_parser::from_preset("-test-canfield");

    game_state gs(sr, string_il{
            {"AC"},{},{},{}, // Foundations
            {},{}, // Stock-waste
            {}, // Reserve
            {"2C","AH"},{"3H"},{"3C","2D"} // Tableau piles
    });

    vector<move> actual_moves = gs.get_legal_moves();
    vector<move> exp_moves = {
            move(move::mtype::built_group, 7, 8, 2),
            move(move::mtype::regular, 7, 1, 1),
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(Canfield, MoveWholePileNotSpace) {
    sol_rules sr = rules_parser::from_preset("-test-canfield");

    game_state gs(sr, string_il{
            {"3C"},{},{},{}, // Foundations
            {},{}, // Stock-waste
            {}, // Reserve
            {"2H","3S","AH"},{},{"2D"} // Tableau piles
    });

    vector<move> actual_moves = gs.get_legal_moves();
    vector<move> exp_moves = {};

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(Canfield, AutoReserve) {
    sol_rules sr = rules_parser::from_preset("-test-canfield");

    game_state gs(sr, string_il{
            {"2C"},{},{},{}, // Foundations
            {"3D","2D"},{}, // Stock-waste
            {"3H"}, // Reserve
            {"2H","3S","AH"},{},{"AC"} // Tableau piles
    });

    vector<move> actual_moves = gs.get_legal_moves();
    vector<move> exp_moves = {};

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;

    boost::optional<move> actual_dom_move = gs.get_dominance_move();
    boost::optional<move> exp_dom_move = move(move::mtype::regular, 6, 8, 1, false, false, true);

    ASSERT_EQ(exp_dom_move, actual_dom_move) << *actual_dom_move;
}

TEST(Canfield, AutoReserveThenWaste) {
    sol_rules sr = rules_parser::from_preset("-test-canfield");

    game_state gs(sr, string_il{
            {"3C"},{},{},{}, // Foundations
            {"2D"},{"2S"}, // Stock-waste
            {}, // Reserve
            {"2H","3S","AH"},{},{"AD"} // Tableau piles
    });

    vector<move> actual_moves = gs.get_legal_moves();
    vector<move> exp_moves = {
            move(move::mtype::stock_k_plus, 4, 8, 0),
            move(move::mtype::stock_k_plus, 4, 8, 1)
    };

    ASSERT_TRUE(test_helper::moves_eq(exp_moves, actual_moves)) << actual_moves;

    boost::optional<move> actual_dom_move = gs.get_dominance_move();
    boost::optional<move> exp_dom_move = boost::none;

    ASSERT_EQ(exp_dom_move, actual_dom_move) << *actual_dom_move;
}
