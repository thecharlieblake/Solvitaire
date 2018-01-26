//
// Created by thecharlesblake on 1/10/18.
//

#include <algorithm>

#include <boost/functional/hash.hpp>

#include "global_cache.h"

using namespace std;
using namespace boost;

typedef sol_rules::build_policy pol;
typedef sol_rules::stock_deal_type sdt;


bool global_cache::insert(const std::vector<pile>& piles) {
    return u_set.insert(piles).second;
}

bool global_cache::contains(const std::vector<pile>& piles) const {
    return u_set.count(piles) > 0;
}

void global_cache::clear() {
    u_set.clear();
}

global_cache::global_cache(const game_state& gs)
        : u_set(0, hasher(gs), predicate(gs)) {
}

hasher::hasher(const game_state& gs) : init_gs(gs) {
}

size_t hasher::operator()(const vector<pile>& piles) const {
    size_t seed = 0;

#ifdef SIMPLE_HASH
    for (game_state::pile_ref pr = 0; pr < piles.size(); pr++) {
        seed += combine(hash_value(piles[pr]));
    }
#else
    // Using addition for commutative hash
    for (game_state::pile_ref pr : init_gs.tableau_piles) {
        combine_commutative(seed, hash_value(piles[pr]));
    }

    // Using hash_combine for order-dependent hash
    for (game_state::pile_ref pr : init_gs.foundations) {
        combine(seed, hash_value(piles[pr]));
    }

    // Commutative
    for (game_state::pile_ref pr : init_gs.reserve) {
        combine_commutative(seed, hash_value(piles[pr]));
    }

    if (init_gs.rules.stock_size > 0) {
        combine(seed, hash_value(piles[init_gs.stock]));

        if (init_gs.rules.stock_deal_t == sdt::WASTE) {
            combine(seed, hash_value(piles[init_gs.waste]));
        }
    }

    if (init_gs.rules.hole) {
        combine(seed, hash_value(piles[init_gs.hole]));
    }

    // Commutative
    for (game_state::pile_ref pr : init_gs.cells) {
        combine_commutative(seed, hash_value(piles[pr]));
    }
#endif

    return seed;
}

size_t hasher::hash_value(pile const& p) const {
    std::size_t seed = 0;

    for(auto first = begin(p.pile_vec); first != end(p.pile_vec); ++first) {
        combine(seed, hash_value(*first));
    }
    return seed;
}

std::size_t hasher::combine(std::size_t& seed, std::size_t value) const {
    return seed ^= value + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

std::size_t hasher::combine_commutative(std::size_t& seed, std::size_t value) const {
    return seed += value;
}

size_t hasher::hash_value(card const& c) const {
    boost::hash<uint8_t> boost_hasher;

#ifdef SIMPLE_HASH
    uint8_t suit_val = static_cast<uint8_t>(c.get_suit());
#else
    uint8_t suit_val;
    switch (init_gs.rules.build_pol) {
        case pol::SAME_SUIT:
            suit_val = static_cast<uint8_t>(c.get_suit());
            break;
        case pol::RED_BLACK:
            suit_val = static_cast<uint8_t>(c.get_colour());
            break;
        default:
            suit_val = 0;
    }
#endif

    auto raw_val = static_cast<uint8_t>(suit_val * 13 + c.get_rank());
    return boost_hasher(raw_val);
}

bool predicate::comp_pile(const pile &x, const pile &y) {
    // Largest piles first
    if (x.size() != y.size()) {
        return x.size() > y.size();
    }
    // Larger card values first
    for (pile::size_type pile_idx = 0; pile_idx < x.size(); pile_idx++) {
        auto x_val = static_cast<uint8_t>(x[pile_idx].get_suit()) * 13
                     + x[pile_idx].get_rank();
        auto y_val = static_cast<uint8_t>(y[pile_idx].get_suit()) * 13
                     + y[pile_idx].get_rank();
        if (x_val != y_val) {
            return x_val > y_val;
        }
    }
    // It is a requirement of std::sort that equal values return false
    return false;
}

bool predicate::operator()(const std::vector<pile>& x,
                                const std::vector<pile>& y) const {
    if (x.size() != y.size()) return false;

    vector<pile> x_piles = x;
    vector<pile> y_piles = y;

    // Orders the piles from largest to smallest
    sort(begin(x_piles), end(x_piles), comp_pile);
    sort(begin(y_piles), end(y_piles), comp_pile);

    // Compares the two for equality
    for (vector<pile>::size_type pile_idx = 0; pile_idx < x_piles.size(); pile_idx++) {
        if (x_piles[pile_idx].size() != y_piles[pile_idx].size()) return false;

        for (auto card_idx = 0; card_idx < x_piles[pile_idx].size(); card_idx++) {
            card cx = x_piles[pile_idx][card_idx];
            card cy = y_piles[pile_idx][card_idx];

            if (cx.get_rank() != cy.get_rank()) return false;

#ifdef SIMPLE_HASH
            if (cx.get_suit() != cy.get_suit()) return false;
#else
            switch (init_gs.rules.build_pol) {
                case pol::SAME_SUIT:
                    if (cx.get_suit() != cy.get_suit()) return false;
                    break;
                case pol::RED_BLACK:
                    if (cx.get_colour() != cy.get_colour()) return false;
                    break;
                default:
                    break;
            }
#endif
        }
    }

    return true;
}

predicate::predicate(const game_state& gs) : init_gs(gs) {
}
