//
// Created by thecharlesblake on 1/10/18.
//

#include <algorithm>

#include "global_cache.h"

using namespace std;
using namespace boost;

typedef sol_rules::build_policy pol;
typedef sol_rules::stock_deal_type sdt;

bool global_cache::insert(const game_state& gs) {
    return u_set.insert(gs).second;
}

bool global_cache::contains(const game_state& gs) const {
    return u_set.count(gs) > 0;
}

void global_cache::clear() {
    u_set.clear();
}

size_t hash_value(card const& c) {
    boost::hash<uint8_t> hasher;

#ifdef SIMPLE_HASH
    uint8_t suit_val = static_cast<uint8_t>(c.get_suit());
#else
    uint8_t suit_val;
    switch (game_state::rules.build_pol) {
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
    return hasher(raw_val);
}

size_t hash_value(pile const& p) {
    return hash_range(begin(p.pile_vec), end(p.pile_vec));
}

size_t hash_value(game_state const& gs) {
    size_t seed = 0;

#ifdef SIMPLE_HASH
    for (game_state::pile_ref pr = 0; pr < gs.piles.size(); pr++) {
        hash_combine(seed, gs.piles[pr]);
    }
#else
    boost::hash<pile> pile_hasher;

    // Using addition for commutative hash
    for (game_state::pile_ref pr : game_state::tableau_piles) {
        seed += pile_hasher(gs.piles[pr]);
    }

    // Using hash_combine for order-dependent hash
    for (game_state::pile_ref pr : game_state::foundations) {
        hash_combine(seed, gs.piles[pr]);
    }

    // Commutative
    for (game_state::pile_ref pr : game_state::reserve) {
        seed += pile_hasher(gs.piles[pr]);
    }

    if (game_state::rules.stock_size > 0) {
        hash_combine(seed, gs.piles[game_state::stock]);
    }

    if (game_state::rules.stock_deal_t == sdt::WASTE) {
        hash_combine(seed, gs.piles[game_state::waste]);
    }

    if (game_state::rules.hole) {
        hash_combine(seed, gs.piles[game_state::hole]);
    }

    // Commutative
    for (game_state::pile_ref pr : game_state::cells) {
        seed += pile_hasher(gs.piles[pr]);
    }
#endif

    return seed;
}

bool game_state_pred::comp_pile(const pile &x, const pile &y) {
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

bool game_state_pred::operator()(const game_state& x,
                                const game_state& y) const {
    if (x.piles.size() != y.piles.size()) return false;

    vector<pile> x_piles = x.piles;
    vector<pile> y_piles = y.piles;

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
            switch (game_state::rules.build_pol) {
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
