//
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>

#include "test_helper.h"
#include "../main/game/game_state.h"
#include "../main/game/global_cache.h"

typedef sol_rules::build_policy pol;

TEST(FoundationsDominance, SameSuit) {
    test_helper::run_foundations_dominance_test(pol::SAME_SUIT, {
            "AC","2C","3C","4C","AH","2H","3H","4H","AS","2S","3S","4S","AD",
            "2D","3D","4D"});
}

TEST(FoundationsDominance, RedBlack) {
    test_helper::run_foundations_dominance_test(pol::RED_BLACK, {
            "AC","2C","AH","2H","AS","3H","2S","AD","3C","4H","3S","2D","4C",
            "4S","3D","4D"});
}

TEST(FoundationsDominance, AnySuit) {
    test_helper::run_foundations_dominance_test(pol::ANY_SUIT, {
            "AC","2C","AH","2H","AS","2S","AD","3C","3H","3S","2D","4C","4H",
            "4S","3D","4D"});
}
