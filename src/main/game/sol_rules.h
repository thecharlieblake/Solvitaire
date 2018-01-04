//
// Created by thecharlesblake on 10/28/17.
//

#ifndef SOLVITAIRE_SOL_RULES_H
#define SOLVITAIRE_SOL_RULES_H

#include <string>

#include <rapidjson/document.h>

#include "card.h"

struct sol_rules {
    // Tableau policies
    enum class build_order {
        DESCENDING,
        ASCENDING
    };
    enum class build_policy {
        NO_BUILD,
        SAME_SUIT,
        RED_BLACK,
        ANY_SUIT
    };
    enum class spaces_policy {
        NO_BUILD,
        ANY
    };

    uint8_t tableau_pile_count;
    build_order build_ord;
    build_policy build_pol;
    spaces_policy spaces_pol;
    card::rank_t max_rank;
    bool hole;
    bool foundations;
    bool foundations_init_card;
    uint8_t cells;
    uint8_t stock_size;
};

#endif //SOLVITAIRE_SOL_RULES_H
