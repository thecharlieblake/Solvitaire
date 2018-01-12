//
// Created by thecharlesblake on 1/10/18.
//

#include <boost/functional/hash.hpp>

#include "global_cache.h"

using namespace std;
using namespace boost;

typedef sol_rules::build_policy pol;
typedef sol_rules::stock_deal_type sdt;
typedef global_cache cache;

global_cache::global_cache(const game_state init_game_state) {
    global_cache::init_gs = init_game_state;
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
    switch (cache::init_gs.rules.build_pol) {
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

size_t hash_value(pile const& p) {
    return hash_range(begin(p.pile_vec), end(p.pile_vec));
}

size_t hash_value(std::vector<pile> const& piles) {
    size_t seed = 0;

    // Using addition for commutative hash
    for (game_state::pile_ref pr : cache::init_gs.tableau_piles) {
        seed += hash_value(piles[pr]);
    }

    // Using hash_combine for order-dependent hash
    for (game_state::pile_ref pr : cache::init_gs.foundations) {
        hash_combine(seed, piles[pr]);
    }

    // Commutative
    for (game_state::pile_ref pr : cache::init_gs.reserve) {
        seed += hash_value(piles[pr]);
    }

    if (cache::init_gs.rules.stock_size > 0) {
        hash_combine(seed, piles[cache::init_gs.stock]);
    }

    if (cache::init_gs.rules.stock_deal_t == sdt::WASTE) {
        hash_combine(seed, piles[cache::init_gs.waste]);
    }

    if (cache::init_gs.rules.stock_deal_t == sdt::WASTE) {
        hash_combine(seed, piles[cache::init_gs.waste]);
    }

    if (cache::init_gs.rules.hole) {
        hash_combine(seed, piles[cache::init_gs.hole]);
    }

    // Commutative
    for (game_state::pile_ref pr : cache::init_gs.cells) {
        seed += hash_value(piles[pr]);
    }

    return seed;
}
