//
// Created by thecharlesblake on 1/10/18.
//

#ifndef SOLVITAIRE_GLOBAL_CACHE_H
#define SOLVITAIRE_GLOBAL_CACHE_H

#include <vector>
#include <unordered_set>

#include "sol_rules.h"
#include "game_state.h"

class predicate {
public:
    predicate(const game_state&);
    bool operator() (const std::vector<pile>&, const std::vector<pile>&) const;
private:
    const game_state& init_gs;
};

struct hasher {
    explicit hasher(const game_state&);
    std::size_t operator() (const std::vector<pile>&) const;

    std::size_t hash_value(const card&) const;
    std::size_t hash_value(const pile&) const;
    std::size_t combine(std::size_t&, std::size_t) const;
    std::size_t combine_commutative(std::size_t&, std::size_t) const;

    const game_state& init_gs;
};

class global_cache {
public:
    explicit global_cache(const game_state&);
    bool insert(const game_state&);
    bool contains(const game_state&) const;
    void clear();

private:
    static std::vector<pile> get_ordered_vec(const game_state&);

    std::unordered_set<
            std::vector<pile>,
            hasher,
            predicate
    > u_set;
};

#endif //SOLVITAIRE_GLOBAL_CACHE_H
