//
// Created by thecharlesblake on 10/28/17.
//

#ifndef SOLVITAIRE_SOL_RULES_H
#define SOLVITAIRE_SOL_RULES_H

#include <string>

#include <rapidjson/document.h>

struct sol_rules {
    sol_rules();
    
    // Tableau policies
    enum class build_policy {
        NO_BUILD,
        SAME_SUIT,
        RED_BLACK,
        ANY_SUIT
    };
    enum class spaces_policy {
        NO_BUILD,
        KINGS,
        ANY
    };
    enum class stock_deal_type {
        WASTE,
        TABLEAU_PILES
    };

    uint8_t tableau_pile_count;
    build_policy build_pol;
    spaces_policy spaces_pol;
    bool move_built_group;
    build_policy built_group_pol;
    bool two_decks;
    uint8_t max_rank;
    bool hole;
    bool foundations;
    bool foundations_init_card;
    bool foundations_removable;
    bool foundations_comp_piles;
    bool diagonal_deal;
    uint8_t cells;
    uint8_t stock_size;
    stock_deal_type stock_deal_t;
    uint8_t stock_deal_count;
    bool stock_redeal;
    uint8_t reserve_size;
    bool reserve_stacked;
};

#endif //SOLVITAIRE_SOL_RULES_H
