//
// Created by thecharlesblake on 12/26/17.
//

#include <rapidjson/document.h>
#include <gtest/gtest.h>

#include "test_helper.h"
#include "../main/game/game_state.h"
#include "../main/solver/solver.h"
#include "../main/input-output/input/json-parsing/json_helper.h"
#include "../main/input-output/input/json-parsing/rules_parser.h"

#define ASSERT_(t_f, statement) ASSERT_TRUE((statement) == (t_f))

using namespace rapidjson;
using namespace std;

bool test_helper::is_solvable(const std::string& input_file, const std::string& preset_type) {
    const Document in_doc = json_helper::get_file_json(input_file);
    const sol_rules rules = rules_parser::from_preset(preset_type);

    game_state gs(rules, in_doc);
    solver sol(gs);

    return sol.run();
}

void test_helper::run_card_cache_test(sol_rules::build_policy bp,
                                      bool assert_dif_col,
                                      bool assert_same_col) {
    sol_rules rules;
    rules.tableau_pile_count = 1;
    rules.build_pol = bp;

    for (uint8_t suit_idx = 0; suit_idx < 4; suit_idx++) {
        for (card::rank_t rank = 1; rank <= 13; rank++) {
            // c2 is the same as c_ but with a different suit
            const card c = card(suit_idx, rank);
            const card c_dif_col = card(uint8_t((suit_idx + 1) % 4), rank);
            const card c_same_col = card(uint8_t((suit_idx + 2) % 4), rank);
            const card c_dif_rank = card(suit_idx, card::rank_t((rank % 13) + 1));

            const game_state s = game_state(rules, initializer_list<pile>{{c}});
            const game_state s_dif_suit(rules, initializer_list<pile>{{c_dif_col}});
            const game_state s_same_col(rules, initializer_list<pile>{{c_same_col}});
            const game_state s_dif_rank(rules, initializer_list<pile>{{c_dif_rank}});

            global_cache cache(s);

            cache.insert(s);

            ASSERT_TRUE(cache.contains(s)) << "Contains self";

            ASSERT_(assert_dif_col, hasher(s)(cached_game_state(s))
                                    == hasher(s)(cached_game_state(s_dif_suit)))
                    << "Insert: " << c.to_string()
                    << ", Comp: " << c_dif_col.to_string();
            ASSERT_(assert_dif_col, cache.contains(s_dif_suit))
                    << "Insert: " << c.to_string()
                    << ", Comp: " << c_dif_col.to_string();

            ASSERT_(assert_same_col, hasher(s)(cached_game_state(s))
                                     == hasher(s)(cached_game_state(s_same_col)))
                    << "Insert: " << c.to_string()
                    << ", Comp: " << c_same_col.to_string();
            ASSERT_(assert_same_col, cache.contains(s_same_col))
                    << "Insert: " << c.to_string()
                    << ", Comp: " << c_same_col.to_string();

            ASSERT_FALSE(hasher(s)(cached_game_state(s))
                         == hasher(s)(cached_game_state(s_dif_rank)))
                    << "Insert: " << c.to_string()
                    << ", Comp: " << c_dif_rank.to_string();
            ASSERT_FALSE(cache.contains(s_dif_rank))
                    << "Insert: " << c.to_string()
                    << ", Comp: " << c_dif_rank.to_string();
        }
    }
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

    solver sol = solver(game_state(gs));
    sol.run();

    solver::node const *n = &sol.get_search_tree();
    for (card c : cards) {
        n = &n->children.back();
        ASSERT_TRUE(n->move.is_dominance());
        ASSERT_TRUE(gs.get_data()[n->move.from].top_card() == c);
        ASSERT_TRUE(n->move.to >= 0 && n->move.to <= 4);
        gs.make_move(n->move);
    }
    ASSERT_TRUE(n->children.empty());
}

void test_helper::optimisation_validity_test(optimisation op) {
    for (string preset : {
            "simple-alpha-star",
            "simple-bakers-dozen",
            "simple-black-hole",
            "simple-canfield",
            "simple-flower-garden",
            "simple-fortunes-favor",
            "simple-free-cell",
            "simple-somerset",
            "simple-spanish-patience",
            "simple-spider"
    }) {
        vector<bool> solutions_op, solutions_no_op;

        switch (op) {
            case optimisation::AUTO_FOUNDATION_MOVES:
#define NO_AUTO_FOUNDATIONS
                solutions_no_op = run_many_games(preset);
#undef NO_AUTO_FOUNDATIONS
                solutions_op = run_many_games(preset);
                break;
        }

        ASSERT_TRUE(solutions_op == solutions_no_op) << preset;
    }
}

std::vector<bool> test_helper::run_many_games(std::string preset) {
#ifdef NDEBUG
    const int run_count = 2000;
#else
    const int run_count = 5;
#endif

    vector<bool> solutions;
    for (int seed = 0; seed < run_count; seed++) {
        solver sol(game_state(rules_parser::from_preset(preset), seed));
        solutions.push_back(sol.run());
    }

    return solutions;
}
