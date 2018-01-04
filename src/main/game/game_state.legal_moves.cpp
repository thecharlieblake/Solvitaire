//
// Created by thecharlesblake on 1/3/18.
//

#include "game_state.h"

using namespace std;

typedef sol_rules::build_order ord;
typedef sol_rules::build_policy pol;
typedef sol_rules::spaces_policy s_pol;

vector<game_state::move> game_state::get_legal_moves() const {
    // The next legal moves
    vector<move> moves;

    // Cycles through each pile which we may be able to remove a card from
    for (pile_ref rem_ref = 0; rem_ref < piles.size(); rem_ref++) {
        // Never removes a card from the hole, the waste, or an empty pile
        if (rem_ref == hole || piles[rem_ref].empty()) continue;

        // Stock cards can only be moved to the waste
        if (stock != PILE_REF_MAX && rem_ref == stock) {
            moves.emplace_back(stock, waste);
            continue;
        }

        // For each tableau pile, check if moving from the rem pile is a valid
        // tableau move according to the rules
        for (auto add_ref : tableau_piles) {
            if (is_valid_tableau_move(rem_ref, add_ref)) {
                moves.emplace_back(rem_ref, add_ref);
            }
        }

        // Any card can be moved to a cell, so long as the cell isn't full
        for (auto add_ref : cells) {
            if (add_ref != rem_ref && piles[add_ref].empty()) {
                moves.emplace_back(rem_ref, add_ref);
            }
        }

        // For each foundation pile, check if moving from the rem pile is a valid
        // move according to the rules
        for (auto add_ref : foundations) {
            if (is_valid_foundations_move(rem_ref, add_ref)) {
                moves.emplace_back(rem_ref, add_ref);
            }
        }

        // Does the same for the hole
        if (hole != PILE_REF_MAX && is_valid_hole_move(rem_ref)) {
            moves.emplace_back(rem_ref, hole);
        }

        // Note that the stock and the reserve aren't mentioned here, as
        // they cannot be added to
    }

    return moves;
}

// If a tableau pile is empty and the space policy is
// no-build, do not add. Otherwise any card with the right
// suit/rank can be moved to a tableau
bool game_state::is_valid_tableau_move(const pile_ref rem_ref,
                                       const pile_ref add_ref) const {
    if (rem_ref == add_ref) return false;

    else if (rules.build_pol == pol::NO_BUILD) return false;

    else if (piles[add_ref].empty()) {
        return rules.spaces_pol == s_pol::ANY;
    }

    card rem_c = piles[rem_ref].top_card();
    card add_c = piles[add_ref].top_card();

    // Checks violation of same suit policy
    if (rules.build_pol == pol::SAME_SUIT
        && rem_c.get_suit() != add_c.get_suit()) {
        return false;
    }

        // Checks violation of red-black suit policy
    else if (rules.build_pol == pol::RED_BLACK
             && rem_c.get_colour() == add_c.get_colour()) {
        return false;
    }

        // Checks rank
    else if (rules.build_ord == ord::DESCENDING
             && rem_c.get_rank() + 1 != add_c.get_rank()) {
        return false;
    } else if (rules.build_ord == ord::ASCENDING
               && rem_c.get_rank() - 1 != add_c.get_rank()) {
        return false;
    } else {
        return true;
    }
}

bool game_state::is_valid_foundations_move(const pile_ref rem_ref,
                                           const pile_ref add_ref) const {
    if (rem_ref == add_ref) return false;

    card rem_c = piles[rem_ref].top_card();

    // Checks violation of same suit policy
    if (rem_c.get_suit() != card::to_suit(add_ref - foundations.front())) {
        return false;
    }

        // Checks rank
    else {
        card::rank_t add_rank = piles[add_ref].empty()
                                ? card::rank_t(0)
                                : piles[add_ref].top_card().get_rank();
        return rem_c.get_rank() - 1 == add_rank;
    }
}

bool game_state::is_valid_hole_move(const pile_ref rem_ref) const {
    if (rem_ref == hole) return false;
    card::rank_t rank = piles[rem_ref].top_card().get_rank();
    card::rank_t hole_rank = piles[hole].top_card().get_rank();
    return rank + 1 == hole_rank
           || rank - 1 == hole_rank
           || (rank == rules.max_rank && hole_rank == 1)
           || (rank == 1 && hole_rank == rules.max_rank);
}
