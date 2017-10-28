//
// Created by thecharlesblake on 10/28/17.
//

#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

#include "sol_rules.h"

using namespace std;

enum class sol_rules::valid_sol {
    BLACK_HOLE,
    SIMPLE_BLACK_HOLE,
    SPANISH_PATIENCE
};

vector<string> sol_rules::valid_sol_strs = {
    "black-hole",
    "simple-black-hole",
    "simple-patience"
};

sol_rules::sol_rules(std::string sol_type) {
    valid_sol vs = valid_sol_enum(sol_type);

    hole = false;
    foundations = true;

    switch(vs) {
        case valid_sol::BLACK_HOLE:
            tableau_pile_count = 17;
            max_rank = 13;
            hole = true;
            foundations = false;
            break;
        case valid_sol::SIMPLE_BLACK_HOLE:
            tableau_pile_count = 9;
            max_rank = 7;
            hole = true;
            foundations = false;
            break;
        case valid_sol::SPANISH_PATIENCE:
            tableau_pile_count = 13;
            max_rank = 13;
            break;
    }
}

sol_rules::valid_sol sol_rules::valid_sol_enum(const std::string& s) {
    auto sol_type_it =
            find(valid_sol_strs.begin(), valid_sol_strs.end(), s);

    assert(sol_type_it != valid_sol_strs.end());

    int idx = sol_type_it - valid_sol_strs.begin();

    return sol_rules::valid_sol(idx);
}