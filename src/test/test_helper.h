//
// Created by thecharlesblake on 12/26/17.
//

#ifndef SOLVITAIRE_TEST_HELPER_H
#define SOLVITAIRE_TEST_HELPER_H

#include <string>

#include "../main/game/sol_rules.h"
#include "../main/game/card.h"
#include "../main/game/pile.h"
#include "../main/game/move.h"

class test_helper {
public:
    static bool is_solvable(const std::string&, const std::string&);
    static void run_foundations_dominance_test(sol_rules::build_policy policy,
                                               std::vector<card> cards);
    static void expected_moves_test(sol_rules sr, std::initializer_list<pile>, std::vector<move>);
    static bool moves_eq(std::vector<move>&, std::vector<move>&);
};

std::ostream& operator <<(std::ostream&, const move&);
std::ostream& operator <<(std::ostream&, const std::vector<move>&);


#endif //SOLVITAIRE_TEST_HELPER_H
