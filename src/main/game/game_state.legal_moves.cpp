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

    if (rules.move_built_group) {
        get_built_group_moves(moves);
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


///////////////////////
// BUILT-GROUP MOVES //
///////////////////////

void game_state::get_built_group_moves(vector<move>& moves) const {
    assert(rules.build_pol != pol::NO_BUILD);

    // Cycles through each pile to see if it contains a built group of size > 1
    for (auto rem_ref : tableau_piles) {
        if (piles[rem_ref].size() < 2) continue;

        auto built_group_height = get_built_group_height(rem_ref);
        if (built_group_height == 1) continue;

        // We have found a built group, so cycles through each pile to see if
        // this group can be added to it
        for (auto add_ref : tableau_piles) {
            if (add_ref == rem_ref) continue;

            card bg_low = piles[rem_ref].top_card();
            card bg_high = piles[rem_ref][built_group_height - 1];

            // If the pile is empty, and we can move to empty piles, does so
            if (piles[add_ref].empty()) {
                if (rules.spaces_pol == s_pol::ANY) {
                    add_empty_built_group_moves(moves, rem_ref, add_ref, bg_high);
                }
            }
                // Otherwise, checks to see if it is a valid move
            else {
                card add_card = piles[add_ref].top_card();

                // Adds the move to the list of moves, as a special built group move
                if (valid_built_group_move(bg_low, bg_high, add_card)) {
                    add_built_group_move(moves, rem_ref, add_ref);
                }
            }
        }
    }
}

// Finds the size of the built group at the top of a pile
pile::size_type game_state::get_built_group_height(pile_ref ref) const {

    // The index of the currently examined card, the current card, previous card
    // and whether we currently have a valid built group.
    pile::size_type group_card_idx = 0;
    bool built_group = true;

    do {
        card prev_c = piles[ref][group_card_idx++];
        card c = piles[ref][group_card_idx];

        // If the build policy is same suit, makes sure group has same suit
        if (rules.build_pol == sol_rules::build_policy::SAME_SUIT
            && c.get_suit() != prev_c.get_suit()) {
            built_group = false;
        }
            // If the build policy is red black, makes sure group alternates
            // in correct phase
        else if (rules.build_pol == sol_rules::build_policy::RED_BLACK
                 && c.get_colour() == prev_c.get_colour()) {
            built_group = false;
        }

        // Makes sure the rank of the cards is sequential
        built_group = built_group
                      && c.get_rank() == prev_c.get_rank()+1;

    }
        // Continues until group_card_idx points to a card outside the built
        // Group
    while (built_group && group_card_idx < piles[ref].size());

    return group_card_idx;
}

bool game_state::valid_built_group_move(card bg_low, card bg_high, card add_card)
const {
    // The rank of the card we're moving the built group to must be greater than
    // the second-from-top card of the built group, and less than or equal to the
    // card that would come next at the top of the built pile
    if (add_card.get_rank() <= bg_low.get_rank() + 1
        || add_card.get_rank() > bg_high.get_rank() + 1) {
        return false;
    }

    // Checks to see if the suits are correct
    switch (rules.build_pol) {
        case sol_rules::build_policy::SAME_SUIT:
            return bg_low.get_suit() == add_card.get_suit();

        case sol_rules::build_policy::RED_BLACK: {
            bool same_suit = bg_low.get_suit() == add_card.get_suit();
            bool even_diff = (add_card.get_rank() - bg_low.get_rank()) % 2 == 0;
            return (same_suit && even_diff) || (!same_suit && !even_diff);
        }
        default: return true;
    }
}

void game_state::add_built_group_move(vector<move>& moves, pile_ref rem_ref,
                                      pile_ref add_ref) const {
    moves.emplace_back(rem_ref, add_ref, 2);
}

void game_state::add_empty_built_group_moves(vector<move>& moves,
                                             pile_ref rem_ref,
                                             pile_ref add_ref,
                                             card bg_high) const {
    // Loops through each possible built group move to an empty pile and adds it
    // to the list
    pile::size_type card_idx = 1;
    do {
        moves.emplace_back(
                rem_ref, add_ref, static_cast<pile_ref>(card_idx + 1)
        );
    } while (piles[rem_ref][card_idx++] != bg_high);
}
