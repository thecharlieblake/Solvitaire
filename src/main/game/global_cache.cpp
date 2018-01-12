//
// Created by thecharlesblake on 1/10/18.
//

#include "global_cache.h"


typedef sol_rules::build_policy pol;

global_cache::global_cache(const sol_rules& rules) {
    global_cache::rules = rules;
}

bool global_cache::insert(const std::vector<pile>& pile_vec) {
    return u_set.insert(pile_vec).second;
}

bool global_cache::contains(const std::vector<pile>& pile_vec) {
    return u_set.count(pile_vec) > 0;
}

size_t hash_value(card const& c) {
    boost::hash<unsigned char> hasher;

    auto suit_val;
    switch (global_cache::rules.build_pol) {
        case pol::SAME_SUIT:
            suit_val = static_cast<std::underlying_type_t<card::suit_t>>(
                    c.get_suit());
            break;
        case pol::RED_BLACK:
            suit_val = static_cast<std::underlying_type_t<card::colour_t >>(
                    c.get_colour());
            break;
        default:
            suit_val = 0;
            break;
    }

    auto raw_val = static_cast<unsigned char>(suit_val * 13 + c.get_rank());
    return hasher(raw_val);
}
