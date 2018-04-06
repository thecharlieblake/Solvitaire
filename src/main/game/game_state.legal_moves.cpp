//
// Created by thecharlesblake on 1/3/18.
//

#include <boost/optional.hpp>

#include "game_state.h"

using namespace std;
using namespace boost;

typedef sol_rules::build_policy pol;
typedef sol_rules::spaces_policy s_pol;
typedef sol_rules::stock_deal_type sdt;

vector<game_state::move> game_state::get_legal_moves(move parent_move) {
    // The next legal moves
    vector<move> moves;

    // If we have a stock that can deal to the tableau piles, adds this move
    if (rules.stock_size > 0
        && rules.stock_deal_t == sdt::TABLEAU_PILES
        && !piles[stock].empty()) {
        
        moves.emplace_back(get_stock_tableau_move());
    }
    
    // If the stock is empty and we are allowed to re-deal, move the waste
    // back onto the stock
    if (rules.stock_size > 0
        && rules.stock_redeal
        && rules.stock_deal_t == sdt::WASTE
        && piles[stock].empty()
        && !piles[waste].empty()) {
        moves.emplace_back(stock, waste);
    }

    // Cycles through each pile which we may be able to remove a card from
    for (pile_ref rem_ref = 0; rem_ref < piles.size(); rem_ref++) {
        // Never removes a card from the hole, an empty pile, or the pile we
        // just moved a card to
        if ((rules.hole && rem_ref == hole)
            || piles[rem_ref].empty()
            || (rem_ref == parent_move.to
                && parent_move.count <= 1
                && parent_move.from != stock)
                ) continue;

        // If we have a foundations card as the rem_ref
        if (rules.foundations
            && rem_ref >= foundations.front()
            && rem_ref <= foundations.back()) {

            // If the foundations are removable, checks to see if a dominance
            // prevents us from removing from it. Otherwise, skips the pile ref
            if (!rules.foundations_removable
#ifndef NO_AUTO_FOUNDATIONS
                || dominance_blocks_foundation_move(rem_ref)
#endif
                    ) {
                continue;
            }
        }

        // Stock cards can only be moved to the waste (assuming there is a waste)
        if (rules.stock_size > 0 && rem_ref == stock) {
            if (rules.stock_deal_t == sdt::WASTE) {
                uint8_t cards_to_move =
                        min(rules.stock_deal_count, piles[stock].size());
                moves.emplace_back(stock, waste, cards_to_move);
            }
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
        if (rules.hole && is_valid_hole_move(rem_ref)) {
            moves.emplace_back(rem_ref, hole);
        }

        // Note that the stock and the reserve aren't mentioned here, as
        // they cannot be added to
    }

    if (rules.move_built_group) {
        get_built_group_moves(moves);
    }

    // If only complete built piles can be moved to the foundations,
    if (rules.foundations_comp_piles) {
        for (pile_ref tab_pr : tableau_piles) {
            if (is_ordered_pile(tab_pr)) {

                for (pile_ref found_pr : foundations) {
                    if (piles[found_pr].empty()) {
                        moves.emplace_back(tab_pr, found_pr, rules.max_rank);
                        break;
                    }
                }
            }
        }
    }


    return moves;
}

// This is a special kind of move which the solver handles differently. Hence
// the arbitrary second param. We must supply the number of stock cards
// that we will deal, so that the move can be undone in backtracking
game_state::move game_state::get_stock_tableau_move() const {
    pile::size_type stock_moves =
            piles[stock].size() >= tableau_piles.size()
            ? pile::size_type(tableau_piles.size())
            : piles[stock].size();

    assert(stock_moves > 0 && stock_moves <= piles[stock].size());
    return {stock, 255, stock_moves};
}

// If a tableau pile is empty and the space policy is
// no-build, do not add. Otherwise any card with the right
// suit/rank can be moved to a tableau
bool game_state::is_valid_tableau_move(const pile_ref rem_ref,
                                       const pile_ref add_ref) const {
    if (rem_ref == add_ref) {
        return false;
    } else if (rules.build_pol == pol::NO_BUILD) {
        return false;
    } else if (piles[add_ref].empty()) {
        switch(rules.spaces_pol) {
            case sol_rules::spaces_policy::NO_BUILD:
                return false;
            case sol_rules::spaces_policy::KINGS:
                return piles[rem_ref].top_card().get_rank() == 13;
            case sol_rules::spaces_policy::ANY:
                return true;
        }
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
    else if (rem_c.get_rank() + 1 != add_c.get_rank()) {
        return false;
    } else {
        return true;
    }
}

bool game_state::is_valid_foundations_move(const pile_ref rem_ref,
                                           const pile_ref add_ref) const {
    if (rules.foundations_comp_piles || rem_ref == add_ref) return false;

    card rem_c = piles[rem_ref].top_card();

    // Checks violation of same suit policy
    uint8_t suit_idx = (add_ref - foundations.front()) % uint8_t(4);
    if (rem_c.get_suit() != suit_idx) {
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
    assert(rules.built_group_pol != pol::NO_BUILD);

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
                if (rules.spaces_pol == s_pol::ANY ||
                        (rules.spaces_pol == s_pol::KINGS
                         && bg_high.get_rank() == 13)) {
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
        if (rules.built_group_pol == pol::SAME_SUIT
            && c.get_suit() != prev_c.get_suit()) {
            built_group = false;
        }
            // If the build policy is red black, makes sure group alternates
            // in correct phase
        else if (rules.built_group_pol == pol::RED_BLACK
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
    switch (rules.built_group_pol) {
        case pol::SAME_SUIT:
            return bg_low.get_suit() == add_card.get_suit();

        case pol::RED_BLACK: {
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

/////////////////////
// Dominance Moves //
/////////////////////

// Returns a dominance move if one is available
optional<game_state::move> game_state::get_dominance_move() const {
#ifndef NO_AUTO_FOUNDATIONS
    // If there are 2 decks or no foundations, return
    if (!rules.foundations || rules.two_decks)
        return none;

    // Cycles through the piles and sees if any cards can be automatically moved
    // to the foundations
    for (pile_ref pr = 0; pr < piles.size(); pr++) {
        // Don't move foundation cards, hole or stock cards to the foundations
        if ((pr >= foundations.front() && pr <= foundations.back())
            || (rules.hole && pr == hole)
            || (rules.stock_size > 0 && pr == stock)
            || (piles[pr].empty())) {
            continue;
        }

        card c = piles[pr].top_card();
        pile_ref target_foundation = foundations[c.get_suit()];

        // If the card is the right rank and the auto-move boolean is true, then
        // returns the move
        card::rank_t target_rank = piles[target_foundation].empty()
                                   ? card::rank_t(1) :
                                   piles[target_foundation].top_card().get_rank()
                                   + card::rank_t(1);
        if (target_rank == c.get_rank() &&
            auto_foundation_moves[c.get_suit()]) {
            return move(pr, target_foundation, move::dominance_flag);
        }
    }
#endif

    return none;
}

bool game_state::is_ordered_pile(pile_ref pr) const {
    if (piles[pr].size() != rules.max_rank) return false;

    card::suit_t s = piles[pr][0].get_suit();
    for (card::rank_t r = 0; r < rules.max_rank; r++) {
        if (piles[pr][r] != card(s, card::rank_t(r + 1))) return false;
    }
    return true;
}

// For games where the foundations can be removed from, this dominance blocks
// the foundations being removed from if the card that were to be removed would
// go 'automatically' up
bool game_state::dominance_blocks_foundation_move(pile_ref target_pile) {
    assert(!piles[target_pile].empty());

    card target_card = piles[target_pile].top_card();
    piles[target_pile].take();

    bool blocked = is_valid_auto_foundation_move(target_pile);
    piles[target_pile].place(target_card);

    return blocked;
}

bool operator==(const game_state::move& a, const game_state::move& b) {
    return    (a.from  == b.from )
           && (a.to    == b.to   )
           && (a.count == b.count);
}
