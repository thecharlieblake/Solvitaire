//
// Created by thecharlesblake on 10/28/17.
//

#ifndef SOLVITAIRE_SOL_RULES_H
#define SOLVITAIRE_SOL_RULES_H

#include <vector>
#include <string>

#include <boost/optional.hpp>
#include <rapidjson/document.h>

#include "card.h"

class sol_rules {
public:
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

    static const sol_rules from_preset(std::string);
    static const sol_rules from_file(std::string);

    uint8_t tableau_pile_count;
    build_order build_ord;
    build_policy build_pol;
    spaces_policy spaces_pol;
    card::rank_t max_rank;
    bool hole;
    bool foundations;
    uint8_t cells;
    uint8_t stock_size;

private:
    static sol_rules get_default();
    static void modify_sol_rules(sol_rules&, rapidjson::Document&);
};

#endif //SOLVITAIRE_SOL_RULES_H
