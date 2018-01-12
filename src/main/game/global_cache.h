//
// Created by thecharlesblake on 1/10/18.
//

#ifndef SOLVITAIRE_GLOBAL_CACHE_H
#define SOLVITAIRE_GLOBAL_CACHE_H

#include <vector>
#include <unordered_set>

#include "../game/game_state.h"
#include "../game/sol_rules.h"

class global_cache {
public:
    explicit global_cache(const sol_rules&);

    bool insert(const std::vector<pile>&);
    bool contains(const std::vector<pile>&);

    friend std::size_t hash_value(card const&);
private:
    static sol_rules rules;

    std::unordered_set<
            std::vector<pile>,
            boost::hash<std::vector<pile>>
    > u_set;
};

#endif //SOLVITAIRE_GLOBAL_CACHE_H
