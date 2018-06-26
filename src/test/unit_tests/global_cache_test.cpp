//
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>

#include "../test_helper.h"
#include "../../main/game/search-state/game_state.h"
#include "../../main/game/global_cache.h"

typedef sol_rules::build_policy pol;
typedef std::initializer_list<std::initializer_list<std::string>> string_il;

TEST(GlobalCache, CommutativeTableauPiles) {
    sol_rules rules;
    rules.tableau_pile_count = 3;
    rules.build_pol = sol_rules::build_policy::SAME_SUIT;
    game_state gs(rules, string_il{{},{},{}});
    lru_cache cache(gs, 1000);

    cache.insert               (game_state(rules, {{"AC"},{"2D"},{"3H"}}));
    ASSERT_TRUE (cache.contains(game_state(rules, {{"2D"},{"3H"},{"AC"}})));
    ASSERT_FALSE(cache.contains(game_state(rules, {{"3H"},{"2D"},{"3H"}})));

    // Test with empty piles
    cache.clear();
    cache.insert               (game_state(rules, {{"4C"},{"5D"},{}}));
    ASSERT_TRUE (cache.contains(game_state(rules, {{},{"4C"},{"5D"}})));
    ASSERT_FALSE(cache.contains(game_state(rules, {{},{"4C"},{}})));

    // Test with piles of size > 1
    cache.clear();
    cache.insert               (game_state(rules, {{"6C","7D"},{"8C"},{"9D"}}));
    ASSERT_TRUE (cache.contains(game_state(rules, {{"8C"},{"6C","7D"},{"9D"}})));
    ASSERT_FALSE(cache.contains(game_state(rules, {{"8C"},{"6C","KD"},{"9D"}})));
}

TEST(GlobalCache, CommutativeReserve) {
    sol_rules rules;
    rules.reserve_size = 3;
    rules.build_pol = sol_rules::build_policy::SAME_SUIT;
    game_state gs(rules, string_il{{},{},{}});
    lru_cache cache(gs, 1000);

    cache.insert               (game_state(rules, {{"AC"},{"2D"},{"3H"}}));
    ASSERT_TRUE (cache.contains(game_state(rules, {{"2D"},{"3H"},{"AC"}})));
    ASSERT_FALSE(cache.contains(game_state(rules, {{"3H"},{"2D"},{"3H"}})));

    // Test with empty piles
    cache.clear();
    cache.insert               (game_state(rules, {{"4C"},{"5D"},{}}));
    ASSERT_TRUE (cache.contains(game_state(rules, {{},{"4C"},{"5D"}})));
    ASSERT_FALSE(cache.contains(game_state(rules, {{},{"4C"},{}})));

    // Test with piles of size > 1
    cache.clear();
    cache.insert               (game_state(rules, {{"6C","7D"},{"8C"},{"9D"}}));
    ASSERT_TRUE (cache.contains(game_state(rules, {{"8C"},{"6C","7D"},{"9D"}})));
    ASSERT_FALSE(cache.contains(game_state(rules, {{"8C"},{"6C","KD"},{"9D"}})));
}

TEST(GlobalCache, CommutativeCells) {
    sol_rules rules;
    rules.cells = 3;
    rules.build_pol = sol_rules::build_policy::SAME_SUIT;
    game_state gs(rules, string_il{{},{},{}});
    lru_cache cache(gs, 1000);

    cache.insert               (game_state(rules, {{"AC"},{"2D"},{"3H"}}));
    ASSERT_TRUE (cache.contains(game_state(rules, {{"2D"},{"3H"},{"AC"}})));
    ASSERT_FALSE(cache.contains(game_state(rules, {{"3H"},{"2D"},{"3H"}})));

    // Test with empty piles
    cache.clear();
    cache.insert               (game_state(rules, {{"4C"},{"5D"},{}}));
    ASSERT_TRUE (cache.contains(game_state(rules, {{},{"4C"},{"5D"}})));
    ASSERT_FALSE(cache.contains(game_state(rules, {{},{"4C"},{}})));
}
