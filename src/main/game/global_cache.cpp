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


///////////////////////
// CACHED GAME STATE //
///////////////////////

cached_game_state::cached_game_state(const game_state& gs) {
    data.reserve(52+18);  // Enough for each card and up to 18 piles

    if (gs.rules.hole) {
        add_card(gs.piles[gs.hole].top_card(), gs);
    }

    for (game_state::pile_ref pr : gs.cells) {
        add_pile(pr, gs);
    }
    if (gs.rules.cells > 0) {
        add_card_divider();
    }

    if (gs.rules.stock_size > 0) {
        add_pile(gs.stock, gs);
        add_card_divider();

        if (gs.rules.stock_deal_t == sdt::WASTE) {
            add_pile(gs.waste, gs);
            add_card_divider();
        }
    }

    for (game_state::pile_ref pr : gs.reserve) {
        add_pile(pr, gs);
    }
    if (gs.rules.reserve_size > 0) {
        add_card_divider();
    }

    for (game_state::pile_ref pr : gs.tableau_piles) {
        add_pile(pr, gs);
        add_card_divider();
    }
}


void cached_game_state::add_pile(game_state::pile_ref pr, const game_state& gs) {
    for (card c : gs.piles[pr].pile_vec) {
        add_card(c, gs);
    }
}

void cached_game_state::add_card(card c, const game_state& gs) {
    auto& target = data;

    // If the game is a 'hole-based' game, or suit-reduction is on, reduces
    // the cached suit of the card where possible

    bool is_suit_symmetry = gs.rules.hole;
#ifdef SUIT_REDUCTION_STREAMLINER
    is_suit_symmetry = true;
#endif
#ifdef NO_SUIT_SYMMETRY
    is_suit_symmetry = false;
#endif

    if (is_suit_symmetry) {
        switch (gs.rules.build_pol) {
            case pol::SAME_SUIT:
                target.emplace_back(c);
                break;
            case pol::RED_BLACK:
                target.emplace_back(c.get_colour(), c.get_rank());
                break;
            default:
                target.emplace_back(0, c.get_rank());
                break;
        }
    } else {
        target.emplace_back(c);
    }
}

void cached_game_state::add_card_divider() {
    data.emplace_back(0, 0);
}

bool operator==(const cached_game_state& a, const cached_game_state& b) {
    return a.data == b.data;
}



//////////////////
// STATE HASHER //
//////////////////

hasher::hasher(const game_state& gs) : init_gs(gs) {
}

size_t hasher::operator()(const cached_game_state& cgs) const {
    size_t seed = 0;

    for (card d : cgs.data) {
        combine(seed, hash_value(d));
    }
    return seed;
}

std::size_t hasher::combine(std::size_t& seed, std::size_t value) const {
    return seed ^= value + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

size_t hasher::hash_value(card const& c) const {
    boost::hash<uint8_t> boost_hasher;

    // If the game is a 'hole-based' game, or suit-reduction is enabled, hash
    // the reduced suit
    bool is_suit_symmetry = init_gs.rules.hole;
#ifdef SUIT_REDUCTION_STREAMLINER
    is_suit_symmetry = true;
#endif
#ifdef NO_SUIT_SYMMETRY
    is_suit_symmetry = false;
#endif
    
    uint8_t suit_val;
    if (is_suit_symmetry) {
        switch (init_gs.rules.build_pol) {
            case pol::SAME_SUIT:
                suit_val = c.get_suit();
                break;
            case pol::RED_BLACK:
                suit_val = c.get_colour();
                break;
            default:
                suit_val = 0;
        }
    } else {
        suit_val = c.get_suit();
    }

    auto raw_val = static_cast<uint8_t>(suit_val * 13 + c.get_rank());
    return boost_hasher(raw_val);
}



//////////////////
// GLOBAL CACHE //
//////////////////

bool global_cache::insert(const game_state& gs) {
    return u_set.emplace(gs).second;
}

bool global_cache::contains(const game_state& gs) const {
    return u_set.count(cached_game_state(gs)) > 0;
}

void global_cache::clear() {
    u_set.clear();
}

global_cache::global_cache(const game_state& gs)
        : u_set(0, hasher(gs)) {
}
