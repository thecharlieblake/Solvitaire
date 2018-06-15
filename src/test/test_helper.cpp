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
using std::initializer_list;
using std::vector;
using std::ostream;

bool test_helper::is_solvable(const std::string& input_file, const std::string& preset_type) {
    const Document in_doc = json_helper::get_file_json(input_file);
    const sol_rules rules = rules_parser::from_preset(preset_type);

    game_state gs(rules, in_doc);
    solver sol(gs, 1000000);

    return sol.run() == solver::sol_state::solved;
}

void test_helper::run_foundations_dominance_test(sol_rules::build_policy policy,
                                                 std::vector<card> cards) {
    sol_rules rules = rules_parser::from_preset("simple-free-cell");
    rules.build_pol = policy;
    rules.foundations_removable = false;
    rules.cells = 0;

    game_state gs = game_state(rules, std::initializer_list<pile>{
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

    solver::node const *n = &sol.get_search_tree();
    for (card c : cards) {
        n = &n->children.back();
        ASSERT_TRUE(n->mv.type == move::mtype::dominance);
        ASSERT_TRUE(gs.get_data()[n->mv.from].top_card() == c);
        ASSERT_TRUE(n->mv.to >= 0 && n->mv.to <= 4);
        gs.make_move(n->mv);
    }
    ASSERT_TRUE(n->children.empty());
}

void test_helper::run_built_group_test(sol_rules sr,
                                       initializer_list<pile> piles,
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
ostream& operator <<(ostream& os, const move& m) {
    return os << "move:(" << int(m.from)
              << ","    << int(m.to)
              << ","     << int(m.count)
              << ")";
}

ostream& operator <<(ostream& os, const vector<move>& moves) {
    for (const move m : moves) {
        os << m << ",\n";
    }
    return os;
}
