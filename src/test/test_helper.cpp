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
// Created by thecharlesblake on 12/26/17.
//

#include <gtest/gtest.h>

#include "test_helper.h"
#include "../main/game/search-state/game_state.h"
#include "../main/solver/solver.h"
#include "../main/input-output/input/json-parsing/json_helper.h"
#include "../main/input-output/input/json-parsing/rules_parser.h"
#include "../../lib/rapidjson/document.h"

#define ASSERT_(t_f, statement) ASSERT_TRUE((statement) == (t_f))

using namespace rapidjson;
using std::vector;
using std::ostream;

typedef std::initializer_list<std::initializer_list<std::string>> string_il;
typedef game_state::streamliner_options sos;


bool test_helper::is_solvable(const std::string& input_file, const std::string& preset_type) {
    const Document in_doc = json_helper::get_file_json(input_file);
    const sol_rules rules = rules_parser::from_preset(preset_type);

    game_state gs(rules, in_doc, sos::NONE);
    solver sol(gs, 1000000);

    return sol.run().sol_type == solver::result::type::SOLVED;
}

void test_helper::run_foundations_dominance_test(sol_rules::build_policy policy,
                                                 std::vector<card> cards) {
    sol_rules rules = rules_parser::from_preset("-test-free-cell");
    rules.build_pol = policy;
    rules.foundations_removable = false;
    rules.cells = 0;

    game_state gs = game_state(rules, string_il{
            {},
            {},
            {},
            {},
            {"4C","3C","2C","AC"},
            {"4H","3H","2H","AH"},
            {"4S","3S","2S","AS"},
            {"4D","3D","2D","AD"}
    });

    solver sol(gs, 1000000);
    sol.run();

    auto frontier = sol.get_frontier();
    auto i = std::begin(frontier);

    for (card c : cards) {
        i++;
        ASSERT_TRUE(i->mv.dominance_move);
        ASSERT_TRUE(gs.get_data()[i->mv.from].top_card() == c);
        ASSERT_TRUE(i->mv.to >= 0 && i->mv.to <= 4);
        gs.make_move(i->mv);
    }
    ASSERT_TRUE(i->child_moves.empty());
}

void test_helper::expected_moves_test(sol_rules sr,
                                      string_il piles,
                                      vector<move> exp_moves) {
    game_state gs(sr, piles);
    vector<move> actual_moves = gs.get_legal_moves();

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

bool test_helper::moves_eq(vector<move>& exp_moves, vector<move>& actual_moves) {
    if (exp_moves.size() != actual_moves.size()) return false;

    for (move m : actual_moves) {
        auto it = find(begin(exp_moves), end(exp_moves), m);
        bool found = it != end(exp_moves);

        if (found) exp_moves.erase(it);
        else return false;
    }

    return true;
}

void test_helper::k_plus_stock_test
        ( sol_rules sr
        , string_il piles
        , vector<move> exp_moves
        , move m
        , pile::size_type stock_size
        , pile::size_type waste_size
        , card top_stock
        , card top_waste
        ) {

    game_state gs(sr, piles);
    vector<move> actual_moves = gs.get_legal_moves();

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;

    gs.make_move(m);

    ASSERT_EQ(gs.get_data()[gs.stock].size(), stock_size) << gs.get_data()[gs.stock].size();
    ASSERT_EQ(gs.get_data()[gs.waste].size(), waste_size) << gs.get_data()[gs.waste].size();

    if (top_stock == card())
        ASSERT_TRUE(gs.get_data()[gs.stock].empty());
    else
        ASSERT_EQ(gs.get_data()[gs.stock].top_card(), top_stock ) << gs.get_data()[gs.stock].top_card().to_string();

    if (top_waste == card())
        ASSERT_TRUE(gs.get_data()[gs.waste].empty());
    else
        ASSERT_EQ(gs.get_data()[gs.waste].top_card(), top_waste ) << gs.get_data()[gs.waste].top_card().to_string();

    gs.undo_move(m);

    ASSERT_EQ(gs.get_data(), game_state(sr, piles).get_data()) << gs;
}

uint8_t test_helper::cards_in_founds(game_state& gs) {
    uint8_t sz = 0;
    for (auto found : gs.foundations) {
        sz += gs.piles[found].size();
    }
    return sz;
}

ostream& operator <<(ostream& os, const move& m) {
    return os << "move:(" << int(m.from)
              << ","    << int(m.to)
              << ","     << int(m.count)
              << "," << m.reveal_move
              << ")";
}

ostream& operator <<(ostream& os, const vector<move>& moves) {
    for (const move m : moves) {
        os << m << ",\n";
    }
    return os;
}
