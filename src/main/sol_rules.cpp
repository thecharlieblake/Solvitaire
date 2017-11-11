//
// Created by thecharlesblake on 10/28/17.
//

#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

#include "sol_rules.h"

using namespace std;

// Static

typedef sol_rules::build_policy pol;

vector<string> sol_rules::valid_sol_strs = {
    "black-hole",
    "simple-black-hole",
    "spanish-patience",
    "simple-spanish-patience",
    "free-cell",
    "simple-free-cell",
    "canfield",
    "simple-canfield"
};

bool sol_rules::is_suit(pol bp) {
    return suit_val(bp) != -1;
}
int sol_rules::suit_val(pol bp) {
    switch (bp) {
        case pol::CLUBS:    return 0;
        case pol::DIAMONDS: return 1;
        case pol::HEARTS:   return 2;
        case pol::SPADES:   return 3;
        default:            return -1;
    }
}

// Non-static

sol_rules::sol_rules(std::string sol_type) {
    valid_sol vs = valid_sol_enum(sol_type);

    // Default game values
    build_ord = build_order::DESCENDING;
    build_pol = build_policy::ANY_SUIT;
    max_rank = 13;
    hole = false;
    foundations = true;
    cells = 0;
    reserve_size = 0;
    stock_size = 0;

    // Game-specific values
    switch(vs) {
        case valid_sol::BLACK_HOLE:
            tableau_pile_count = 17;
            build_ord = build_order::NO_BUILD;
            hole = true;
            foundations = false;
            cells = 0;
            break;
        case valid_sol::SIMPLE_BLACK_HOLE:
            tableau_pile_count = 9;
            build_ord = build_order::NO_BUILD;
            max_rank = 7;
            hole = true;
            foundations = false;
            break;
        case valid_sol::SPANISH_PATIENCE:
            tableau_pile_count = 13;
            break;
        case valid_sol::SIMPLE_SPANISH_PATIENCE:
            tableau_pile_count = 5;
            max_rank = 5;
            break;
        case valid_sol::FREE_CELL:
            tableau_pile_count = 8;
            cells = 4;
            break;
        case valid_sol::SIMPLE_FREE_CELL:
            tableau_pile_count = 3;
            max_rank = 3;
            cells = 1;
            break;
        case valid_sol::CANFIELD:
            tableau_pile_count = 4;
            build_pol = build_policy::RED_BLACK;
            reserve_size = 14;
            stock_size = 34;
            break;
        case valid_sol::SIMPLE_CANFIELD:
            tableau_pile_count = 3;
            build_pol = build_policy::RED_BLACK;
            max_rank = 3;
            reserve_size = 3;
            stock_size = 6;
            break;
        default:
            assert(false);
    }
}

sol_rules::valid_sol sol_rules::valid_sol_enum(const std::string& s) {
    auto sol_type_it =
            find(valid_sol_strs.begin(), valid_sol_strs.end(), s);

    assert(sol_type_it != valid_sol_strs.end());

    long idx = sol_type_it - valid_sol_strs.begin();

    return sol_rules::valid_sol(idx);
}