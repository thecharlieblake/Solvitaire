//
// Created by thecharlesblake on 1/10/18.
//

#include "global_cache.h"


global_cache::global_cache(const sol_rules&) {}

bool global_cache::insert(const std::vector<pile>& pile_vec) {
    return u_set.insert(pile_vec).second;
}

bool global_cache::contains(const std::vector<pile>& pile_vec) {
    return u_set.count(pile_vec) > 0;
}
