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
    "simple-spanish-patience"
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


    switch(vs) {
        case valid_sol::BLACK_HOLE:
            tableau_pile_count = 17;
            build_ord = build_order::NO_BUILD;
            max_rank = 13;
            hole = true;
            foundations = false;
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
            build_ord = build_order::DESCENDING;
            max_rank = 13;
            hole = false;
            foundations = true;
            break;
        case valid_sol::SIMPLE_SPANISH_PATIENCE:
            tableau_pile_count = 5;
            build_ord = build_order::DESCENDING;
            max_rank = 5;
            hole = false;
            foundations = true;
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