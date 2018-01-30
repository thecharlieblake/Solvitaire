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

            ASSERT_(assert_dif_col, hasher(s)(s.get_data()) == hasher(s)(s_dif_suit.get_data()))
                    << "Insert: " << c.to_string()
                    << ", Comp: " << c_dif_col.to_string();
            ASSERT_(assert_dif_col, cache.contains(s_dif_suit))
                    << "Insert: " << c.to_string()
                    << ", Comp: " << c_dif_col.to_string();

            ASSERT_(assert_same_col, hasher(s)(s.get_data()) == hasher(s)(s_same_col.get_data()))
                    << "Insert: " << c.to_string()
                    << ", Comp: " << c_same_col.to_string();
            ASSERT_(assert_same_col, cache.contains(s_same_col))
                    << "Insert: " << c.to_string()
                    << ", Comp: " << c_same_col.to_string();

            ASSERT_FALSE(hasher(s)(s.get_data()) == hasher(s)(s_dif_rank.get_data()))
                    << "Insert: " << c.to_string()
                    << ", Comp: " << c_dif_rank.to_string();
            ASSERT_FALSE(cache.contains(s_dif_rank))
                    << "Insert: " << c.to_string()
                    << ", Comp: " << c_dif_rank.to_string();
        }
    }
}
