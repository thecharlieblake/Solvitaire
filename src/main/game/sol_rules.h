//
// Created by thecharlesblake on 10/28/17.
//

#ifndef SOLVITAIRE_SOL_RULES_H
#define SOLVITAIRE_SOL_RULES_H

#include <string>
#include <vector>

#include "../../../lib/rapidjson/document.h"

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
    enum class accordion_policy {
        SAME_RANK,
        SAME_SUIT,
        RED_BLACK,
        ANY_SUIT
    };
    enum class stock_deal_type {
        WASTE,
        TABLEAU_PILES
    };
    enum class face_up_policy {
        ALL,
        TOP_CARDS
    };
    enum class direction {
        LEFT,
        RIGHT,
        BOTH
    };
    enum class built_group_type {
        YES,
        NO,
        WHOLE_PILE
    };
    enum class foundations_init_type {
        NONE,
        ONE,
        ALL
    };

    uint8_t tableau_pile_count;
    build_policy build_pol;
    spaces_policy spaces_pol;
    built_group_type move_built_group;
    build_policy built_group_pol;
    bool two_decks;
    uint8_t max_rank;
    bool hole;
    bool foundations_present;
    foundations_init_type foundations_init_cards;
    bool foundations_removable;
    bool foundations_only_comp_piles;
    bool diagonal_deal;
    uint8_t cells;
    uint8_t stock_size;
    stock_deal_type stock_deal_t;
    uint8_t stock_deal_count;
    bool stock_redeal;
    uint8_t reserve_size;
    bool reserve_stacked;
    face_up_policy face_up;
    uint8_t sequence_count;
    direction sequence_direction;
    build_policy sequence_build_pol;
    uint8_t accordion_size;
    std::vector<std::pair<direction, uint8_t>> accordion_moves;
    std::vector<accordion_policy> accordion_pol;
};

#endif //SOLVITAIRE_SOL_RULES_H
