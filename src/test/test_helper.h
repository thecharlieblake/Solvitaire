//
// Created by thecharlesblake on 12/26/17.
//

#ifndef SOLVITAIRE_TEST_HELPER_H
#define SOLVITAIRE_TEST_HELPER_H

#include <string>

#include "../main/game/sol_rules.h"
#include "../main/game/card.h"

class test_helper {
public:
    enum class optimisation {
        AUTO_FOUNDATION_MOVES
    };

    static bool is_solvable(const std::string&, const std::string&);
    static void run_card_cache_test(sol_rules::build_policy, bool, bool);

    static void run_foundations_dominance_test(sol_rules::build_policy policy,
                                               std::vector<card> cards);

    static void optimisation_validity_test(optimisation);

    static std::vector<bool> run_many_games(std::string);
};


#endif //SOLVITAIRE_TEST_HELPER_H
