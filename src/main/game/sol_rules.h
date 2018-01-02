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
    enum class valid_sol {
        BLACK_HOLE,
        SIMPLE_BLACK_HOLE,
        SPANISH_PATIENCE,
        SIMPLE_SPANISH_PATIENCE,
        FREE_CELL,
        SIMPLE_FREE_CELL,
        CANFIELD,
        SIMPLE_CANFIELD
    };
    enum class build_order {
        NO_BUILD,
        DESCENDING,
        ASCENDING,
        BOTH,
        SINGLE_CARD,
        ANY
    };
    enum class build_policy {
        CLUBS,
        DIAMONDS,
        HEARTS,
        SPADES,
        ANY_SUIT,
        SAME_SUIT,
        RED_BLACK,
        N_A
    };
    enum class spaces_policy {
        NO_BUILD,
        ANY
    };

    static bool is_suit(build_policy);
    static card::suit_t suit_val(build_policy);

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
    uint8_t reserve_size;
    uint8_t stock_size;

private:
    static sol_rules get_default();
    static void modify_sol_rules(sol_rules&, rapidjson::Document&);
    static void parse_err(const std::string&);

    static valid_sol valid_sol_enum(const std::string&);
};

#endif //SOLVITAIRE_SOL_RULES_H
