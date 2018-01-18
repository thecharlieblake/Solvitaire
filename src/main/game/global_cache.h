//
// Created by thecharlesblake on 1/10/18.
//

#ifndef SOLVITAIRE_GLOBAL_CACHE_H
#define SOLVITAIRE_GLOBAL_CACHE_H

#include <vector>
#include <unordered_set>

#include <boost/functional/hash.hpp>

#include "sol_rules.h"
#include "game_state.h"

class global_cache {
public:
    bool insert(const game_state&);
    bool contains(const game_state&) const;
    void clear();

    struct game_state_pred {
        static bool comp_pile(const pile& x, const pile& y);
        bool operator() (const game_state& x, const game_state& y)
        const;
    };
private:
    std::unordered_set<
            game_state,
            boost::hash<game_state>,
            game_state_pred
    > u_set;
};

#endif //SOLVITAIRE_GLOBAL_CACHE_H
