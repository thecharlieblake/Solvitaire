//
// Created by thecharlesblake on 1/10/18.
//

#ifndef SOLVITAIRE_GLOBAL_CACHE_H
#define SOLVITAIRE_GLOBAL_CACHE_H

#include <vector>
#include <unordered_set>
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/hashed_index.hpp>

#include "sol_rules.h"
#include "search-state/game_state.h"

struct cached_game_state {
    typedef std::vector<card> state_data;
    typedef state_data::size_type size_type;

    explicit cached_game_state(const game_state&);
    void add_pile(pile::ref, const game_state&);
    void add_card(card, const game_state&);
    void add_card_divider();

    state_data data;
};

bool operator==(const cached_game_state&, const cached_game_state&);

struct hasher {
    explicit hasher(const game_state&);
    std::size_t operator() (const cached_game_state&) const;

    std::size_t hash_value(const card&) const;
    std::size_t combine(std::size_t&, std::size_t) const;

    const game_state& init_gs;
};

class global_cache {
public:
    virtual ~global_cache() = default;
    virtual bool insert(const game_state&) = 0;
    virtual bool contains(const game_state&) const = 0;
    virtual void clear() = 0;
};

class unlimited_cache : public global_cache {
public:
    explicit unlimited_cache(const game_state&);
    bool insert(const game_state&) override;
    bool contains(const game_state&) const override;
    void clear() override;

private:
    std::unordered_set<
            cached_game_state,
            hasher
    > cache;
};

class lru_cache : public global_cache {
    typedef boost::multi_index::multi_index_container<
            cached_game_state,
            boost::multi_index::indexed_by<
                    boost::multi_index::sequenced<>,
                    boost::multi_index::hashed_unique<
                            boost::multi_index::identity<cached_game_state>,
                            hasher
                    >
            >
    > item_list;
public:
    explicit lru_cache(const game_state&, std::size_t);
    bool insert(const game_state&) override;
    bool contains(const game_state&) const override;
    void clear() override;

private:
    static item_list::ctor_args_list get_init_tuple(const game_state&);

    std::size_t max_num_items;
    item_list cache;
};

#endif //SOLVITAIRE_GLOBAL_CACHE_H
