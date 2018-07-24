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
#include "../main/game/search-state/game_state.h"

class test_helper {
public:
    static bool is_solvable(const std::string&, const std::string&);
    static void run_foundations_dominance_test(sol_rules::build_policy policy,
                                               std::vector<card> cards);
    static void expected_moves_test(sol_rules sr, std::initializer_list<std::initializer_list<std::string>>,
            std::vector<move>);
    static void k_plus_stock_test
            ( sol_rules sr
            , std::initializer_list<std::initializer_list<std::string>>
            , std::vector<move>
            , move
            , pile::size_type
            , pile::size_type
            , card
            , card
            );
    static bool moves_eq(std::vector<move>&, std::vector<move>&);
    static uint8_t cards_in_founds(game_state&);
};

std::ostream& operator <<(std::ostream&, const move&);
std::ostream& operator <<(std::ostream&, const std::vector<move>&);


#endif //SOLVITAIRE_TEST_HELPER_H
