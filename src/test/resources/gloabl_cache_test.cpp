//
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>
#include "../../main/game/global_cache.h"

typedef sol_rules::build_policy pol;

TEST(BuildSameSuit, GlobalCache) {
    sol_rules rules;
    rules.tableau_pile_count = 1;
    rules.build_pol = pol::SAME_SUIT;
    global_cache cache(rules);

    for (uint8_t suit_idx = 0; suit_idx < 4; suit_idx++) {
        for (card::rank_t rank = 1; rank <= 13; rank++) {
            // c2 is the same as c_ but with a different suit
            card c = card(card::to_suit(suit_idx), rank);
            card c_dif_suit = card(card::to_suit(uint8_t((suit_idx + 1) % 4)),
                                   rank);
            card c_dif_rank = card(card::to_suit(suit_idx),
                                   card::rank_t((rank % 13) + 1));

            cache.insert({{c}});

            EXPECT_TRUE(cache.contains({{c}}));
            EXPECT_TRUE(cache.contains({{c_dif_suit}}));
            EXPECT_FALSE(cache.contains({{c_dif_rank}}));
        }
    }
}

TEST(BuildRedBlack, GlobalCache) {
    sol_rules rules;
    rules.tableau_pile_count = 1;
    rules.build_pol = pol::RED_BLACK;
    global_cache cache(rules);

    for (uint8_t suit_idx = 0; suit_idx < 4; suit_idx++) {
        for (card::rank_t rank = 1; rank <= 13; rank++) {
            // c2 is the same as c_ but with a different suit
            card c = card(card::to_suit(suit_idx), rank);
            card c_same_col = card(card::to_suit(uint8_t((suit_idx + 2) % 4)),
                                   rank);
            card c_dif_col = card(card::to_suit(uint8_t((suit_idx + 1) % 4)),
                                  rank);
            card c_dif_rank = card(card::to_suit(suit_idx),
                                   card::rank_t((rank % 13) + 1));

            cache.insert({{c}});

            EXPECT_TRUE(cache.contains({{c}}));
            EXPECT_TRUE(cache.contains({{c_same_col}}));
            EXPECT_FALSE(cache.contains({{c_dif_col}}));
            EXPECT_FALSE(cache.contains({{c_dif_rank}}));
        }
    }
}

TEST(BuildAny, GlobalCache) {
    sol_rules rules;
    rules.tableau_pile_count = 1;
    rules.build_pol = pol::ANY_SUIT;
    global_cache cache(rules);

    for (uint8_t suit_idx = 0; suit_idx < 4; suit_idx++) {
        for (card::rank_t rank = 1; rank <= 13; rank++) {
            // c2 is the same as c_ but with a different suit
            card c = card(card::to_suit(suit_idx), rank);
            card c_same_col = card(card::to_suit(uint8_t((suit_idx + 2) % 4)),
                                   rank);
            card c_dif_col = card(card::to_suit(uint8_t((suit_idx + 1) % 4)),
                                  rank);
            card c_dif_rank = card(card::to_suit(suit_idx),
                                   card::rank_t((rank % 13) + 1));

            cache.insert({{c}});

            EXPECT_TRUE(cache.contains({{c}}));
            EXPECT_TRUE(cache.contains({{c_same_col}}));
            EXPECT_TRUE(cache.contains({{c_dif_col}}));
            EXPECT_FALSE(cache.contains({{c_dif_rank}}));
        }
    }
}

TEST(BuildNone, GlobalCache) {
    sol_rules rules;
    rules.tableau_pile_count = 1;
    rules.build_pol = pol::NO_BUILD;
    global_cache cache(rules);

    for (uint8_t suit_idx = 0; suit_idx < 4; suit_idx++) {
        for (card::rank_t rank = 1; rank <= 13; rank++) {
            // c2 is the same as c_ but with a different suit
            card c = card(card::to_suit(suit_idx), rank);
            card c_same_col = card(card::to_suit(uint8_t((suit_idx + 2) % 4)),
                                   rank);
            card c_dif_col = card(card::to_suit(uint8_t((suit_idx + 1) % 4)),
                                  rank);
            card c_dif_rank = card(card::to_suit(suit_idx),
                                   card::rank_t((rank % 13) + 1));

            cache.insert({{c}});

            EXPECT_TRUE(cache.contains({{c}}));
            EXPECT_TRUE(cache.contains({{c_same_col}}));
            EXPECT_TRUE(cache.contains({{c_dif_col}}));
            EXPECT_FALSE(cache.contains({{c_dif_rank}}));
        }
    }
}

// Commutative tableau piles
// Commutative cells
// Commutative reserve
// Other piles not so

TEST(CommutativeTableauPiles, GlobalCache) {
    sol_rules rules;
    rules.tableau_pile_count = 3;
    rules.build_pol = pol::SAME_SUIT;
    global_cache cache(rules);

    cache.insert               ({{card("AC")},{card("2D")},{card("3H")}});
    EXPECT_TRUE (cache.contains({{card("2D")},{card("3H")},{card("AC")}}));
    EXPECT_FALSE(cache.contains({{card("3H")},{card("2D")},{card("3H")}}));

    // Test with empty piles
    cache.insert               ({{card("4C")},{card("5D")},{}});
    EXPECT_TRUE (cache.contains({{},{card("4C")},{card("5D")}}));
    EXPECT_FALSE(cache.contains({{},{card("4C")},{}}));

    // Test with piles of size > 1
    cache.insert               ({{card("6C"),card("7D")},{card("8C")},{card("9D")}});
    EXPECT_TRUE (cache.contains({{card("8C")},{card("6C"),card("7D")},{card("9D")}}));
    EXPECT_FALSE(cache.contains({{card("8C")},{card("6C"),card("KD")},{card("9D")}}));
}

TEST(CommutativeReserve, GlobalCache) {
    sol_rules rules;
    rules.reserve_size = 3;
    rules.build_pol = pol::SAME_SUIT;
    global_cache cache(rules);

    cache.insert               ({{card("AC")},{card("2D")},{card("3H")}});
    EXPECT_TRUE (cache.contains({{card("2D")},{card("3H")},{card("AC")}}));
    EXPECT_FALSE(cache.contains({{card("3H")},{card("2D")},{card("3H")}}));

    // Test with empty piles
    cache.insert               ({{card("4C")},{card("5D")},{}});
    EXPECT_TRUE (cache.contains({{},{card("4C")},{card("5D")}}));
    EXPECT_FALSE(cache.contains({{},{card("4C")},{}}));

    // Test with piles of size > 1
    cache.insert               ({{card("6C"),card("7D")},{card("8C")},{card("9D")}});
    EXPECT_TRUE (cache.contains({{card("8C")},{card("6C"),card("7D")},{card("9D")}}));
    EXPECT_FALSE(cache.contains({{card("8C")},{card("6C"),card("KD")},{card("9D")}}));
}

TEST(CommutativeCells, GlobalCache) {
    sol_rules rules;
    rules.cells = 3;
    rules.build_pol = pol::SAME_SUIT;
    global_cache cache(rules);

    cache.insert               ({{card("AC")},{card("2D")},{card("3H")}});
    EXPECT_TRUE (cache.contains({{card("2D")},{card("3H")},{card("AC")}}));
    EXPECT_FALSE(cache.contains({{card("3H")},{card("2D")},{card("3H")}}));

    // Test with empty cells
    cache.insert               ({{card("4C")},{card("5D")},{}});
    EXPECT_TRUE (cache.contains({{},{card("4C")},{card("5D")}}));
    EXPECT_FALSE(cache.contains({{},{card("4C")},{}}));
}
