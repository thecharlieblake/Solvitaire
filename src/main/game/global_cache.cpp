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

bool global_cache::insert(const game_state& gs) {
    return u_set.emplace(get_ordered_vec(gs)).second;
}

bool global_cache::contains(const game_state& gs) const {
    return u_set.count(get_ordered_vec(gs)) > 0;
}

void global_cache::clear() {
    u_set.clear();
}

global_cache::global_cache(const game_state& gs)
        : u_set(0, hasher(gs), predicate(gs)) {
}

std::vector<pile> global_cache::get_ordered_vec(const game_state& gs) {
#ifdef ORDER_ON_CACHE
    vector<pile> ordered_vec = gs.get_data();
    if (gs.rules.cells > 0) {
        sort(begin(ordered_vec) + gs.original_cells[0],
             begin(ordered_vec) + gs.original_cells[0] + gs.original_cells.size());
    }
    if (gs.rules.reserve_size > 0) {
        sort(begin(ordered_vec) + gs.original_reserve[0],
             begin(ordered_vec) + gs.original_reserve[0] + gs.original_reserve.size());
    }
    if (gs.rules.tableau_pile_count > 0) {
        sort(begin(ordered_vec) + gs.original_tableau_piles[0],
             begin(ordered_vec) + gs.original_tableau_piles[0] + gs.original_tableau_piles.size());
    }
#else
    const vector<pile>& gs_vec = gs.get_data();
    vector<pile> ordered_vec;
    ordered_vec.reserve(gs_vec.size());

    if (gs.rules.hole)
        ordered_vec.push_back(gs_vec[gs.hole]);

    for (game_state::pile_ref pr : gs.foundations)
        ordered_vec.push_back(gs_vec[pr]);

    for (game_state::pile_ref pr : gs.cells)
        ordered_vec.push_back(gs_vec[pr]);

    if (gs.rules.stock_size > 0) {
        ordered_vec.push_back(gs_vec[gs.stock]);

        if (gs.rules.stock_deal_t == sdt::WASTE) {
            ordered_vec.push_back(gs_vec[gs.waste]);
        }
    }

    for (game_state::pile_ref pr : gs.reserve)
        ordered_vec.push_back(gs_vec[pr]);

    for (game_state::pile_ref pr : gs.tableau_piles)
        ordered_vec.push_back(gs_vec[pr]);

#ifndef NDEBUG
    // Makes sure the piles are in order
    if (gs.original_cells.size() > 1) {
        for (game_state::pile_ref pr : gs.original_cells) {
            assert(ordered_vec[pr] >= ordered_vec[pr+1]);
            if (pr == gs.original_cells[gs.original_cells.size()-2]) break;
        }
    }
    if (gs.original_reserve.size() > 1) {
        for (game_state::pile_ref pr : gs.original_reserve) {
            assert(ordered_vec[pr] >= ordered_vec[pr+1]);
            if (pr == gs.original_reserve[gs.original_reserve.size()-2]) break;
        }
    }
    if (gs.original_tableau_piles.size() > 1) {
        for (game_state::pile_ref pr : gs.original_tableau_piles) {
            assert(ordered_vec[pr] >= ordered_vec[pr+1]);
            if (pr == gs.original_tableau_piles[gs.original_tableau_piles.size()-2]) break;
        }
    }
#endif
#endif

    return ordered_vec;
}



hasher::hasher(const game_state& gs) : init_gs(gs) {
}

size_t hasher::operator()(const vector<pile>& piles) const {
    size_t seed = 0;

#ifdef SIMPLE_HASH
    for (game_state::pile_ref pr = 0; pr < piles.size(); pr++) {
        seed += combine(seed, hash_value(piles[pr]));
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

predicate::predicate(const game_state& gs) : init_gs(gs) {
}

bool predicate::operator()(const vector<pile>& x_piles,
                           const vector<pile>& y_piles) const {
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
