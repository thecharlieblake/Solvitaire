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
    return u_set.emplace(gs).second;
}

bool global_cache::contains(const game_state& gs) const {
    return u_set.count(cached_game_state(gs)) > 0;
}

void global_cache::clear() {
    u_set.clear();
}

global_cache::global_cache(const game_state& gs)
        : u_set(0, hasher(gs), predicate(gs)) {
}



hasher::hasher(const game_state& gs) : init_gs(gs) {
}

size_t hasher::operator()(const cached_game_state& cgs) const {
    size_t seed = 0;

#ifdef NO_REDUCED_STATE
    for (const pile& d : cgs.data) {
#else
    for (card d : cgs.data) {
#endif
        combine(seed, hash_value(d));
    }
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

bool predicate::operator()(const cached_game_state& a,
                           const cached_game_state& b) const {
#ifdef NO_REDUCED_STATE
    for (vector<pile>::size_type pile_idx = 0; pile_idx < a.data.size(); pile_idx++) {
        if (a.data[pile_idx].size() != b.data[pile_idx].size()) return false;

        for (auto card_idx = 0; card_idx < a.data[pile_idx].size(); card_idx++) {
            card cx = a.data[pile_idx][card_idx];
            card cy = b.data[pile_idx][card_idx];

            if (cx.get_rank() != cy.get_rank()) return false;

# ifdef SIMPLE_HASH
            if (cx.get_suit() != cy.get_suit()) return false;
# else
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
# endif
        }
    }
#else
    for (cached_game_state::size_type i = 0; i < a.data.size(); i++) {
        if (a.data[i] != b.data[i]) return false;
    }
#endif

    return true;
}

cached_game_state::cached_game_state(const game_state& gs) {
#ifdef NO_REDUCED_STATE
    const vector<pile>& gs_vec = gs.get_data();

    if (gs.rules.hole)
        data.push_back(gs_vec[gs.hole]);

    for (game_state::pile_ref pr : gs.foundations)
        data.push_back(gs_vec[pr]);

    for (game_state::pile_ref pr : gs.cells)
        data.push_back(gs_vec[pr]);

    if (gs.rules.stock_size > 0) {
        data.push_back(gs_vec[gs.stock]);

        if (gs.rules.stock_deal_t == sdt::WASTE) {
            data.push_back(gs_vec[gs.waste]);
        }
    }

    for (game_state::pile_ref pr : gs.reserve)
        data.push_back(gs_vec[pr]);

    for (game_state::pile_ref pr : gs.tableau_piles)
        data.push_back(gs_vec[pr]);
#else
    if (gs.rules.hole) {
        add_card(gs.piles[gs.hole].top_card(), gs);
    }

    // No need to add foundations, as they are implicit in the other piles

    for (game_state::pile_ref pr : gs.cells) {
        add_pile(gs.piles[pr], gs);
    }
    if (gs.rules.cells > 0) {
        add_card_divider();
    }

    if (gs.rules.stock_size > 0) {
        add_pile(gs.piles[gs.stock], gs);
        add_card_divider();

        if (gs.rules.stock_deal_t == sdt::WASTE) {
            add_pile(gs.piles[gs.waste], gs);
            add_card_divider();
        }
    }

    for (game_state::pile_ref pr : gs.reserve) {
        add_pile(gs.piles[pr], gs);
    }
    if (gs.rules.reserve_size > 0) {
        add_card_divider();
    }

    for (game_state::pile_ref pr : gs.tableau_piles) {
        add_pile(gs.piles[pr], gs);
        add_card_divider();
    }
#endif
}


#ifndef NO_REDUCED_STATE
void cached_game_state::add_pile(const pile& p, const game_state& gs) {
    for (card c : p.pile_vec) {
        add_card(c, gs);
    }
}

void cached_game_state::add_card(card c, const game_state& gs) {
    switch (gs.rules.build_pol) {
# ifdef SIMPLE_HASH
        default:
            data.emplace_back(c);
            break;
# else
        case pol::SAME_SUIT:
            data.emplace_back(c);
            break;
        case pol::RED_BLACK:
            data.emplace_back(c.get_colour(), c.get_rank());
            break;
        default:
            data.emplace_back(0, c.get_rank());
            break;
# endif
    }
}

void cached_game_state::add_card_divider() {
    data.emplace_back(0, 0);
}
#endif
