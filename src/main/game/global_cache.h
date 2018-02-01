//
// Created by thecharlesblake on 1/10/18.
//

#ifndef SOLVITAIRE_GLOBAL_CACHE_H
#define SOLVITAIRE_GLOBAL_CACHE_H

#include <vector>
#include <unordered_set>
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>

#include "sol_rules.h"
#include "game_state.h"

struct cached_game_state {
    typedef std::vector<
#ifdef NO_REDUCED_STATE
            pile
#else
            card
#endif
    > state_data;
    typedef state_data::size_type size_type;

    explicit cached_game_state(const game_state&);
#ifndef NO_REDUCED_STATE
    void add_pile(const pile&, const game_state&);
    void add_card(card, const game_state&);
    void add_card_divider();
#endif

    state_data data;
};

class predicate {
public:
    explicit predicate(const game_state&);
    bool operator() (const cached_game_state&, const cached_game_state&) const;
private:
    const game_state& init_gs;
};

struct hasher {
    explicit hasher(const game_state&);
    std::size_t operator() (const cached_game_state&) const;

    std::size_t hash_value(const card&) const;
    std::size_t hash_value(const pile&) const;
    std::size_t combine(std::size_t&, std::size_t) const;

    const game_state& init_gs;
};

class global_cache {
public:
    explicit global_cache(const game_state&);
    bool insert(const game_state&);
    bool contains(const game_state&) const;
    void clear();

private:
    std::unordered_set<
            cached_game_state,
            hasher,
            predicate
    > u_set;
};

#endif //SOLVITAIRE_GLOBAL_CACHE_H
