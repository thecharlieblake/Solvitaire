//
// Created by thecharlesblake on 12/26/17.
//

#ifndef SOLVITAIRE_TEST_HELPER_H
#define SOLVITAIRE_TEST_HELPER_H

#include <string>

#include "../main/game/sol_rules.h"

class test_helper {
public:
    static bool is_solvable(const std::string&, const std::string&);
    static void run_card_cache_test(sol_rules::build_policy, bool, bool);
};


#endif //SOLVITAIRE_TEST_HELPER_H
