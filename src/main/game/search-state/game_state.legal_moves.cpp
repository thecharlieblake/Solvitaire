//
// Created by thecharlesblake on 07/06/18.
//

#include "game_state.h"

#include <boost/optional/optional.hpp>

using std::vector;
using std::min;
using namespace rapidjson;

typedef sol_rules::build_policy pol;
typedef sol_rules::spaces_policy s_pol;
typedef sol_rules::stock_deal_type sdt;


////////////////////////////////
// MAIN LEGAL MOVE GEN CYCLE ///
////////////////////////////////

// Note that the moves added last here, are tried first
vector<move> game_state::get_legal_moves(move parent_move) {
    vector<move> moves;

    // Attempts stock tableau deal and redeal
    if (rules.stock_size > 0) {
        if (stock_can_deal_tableau())
            moves.emplace_back(get_stock_tableau_move());
        else if (stock_can_redeal())
            moves.emplace_back(move::mtype::redeal);
    }

    // Cycles through all of the piles from which we want to remove cards
    for (pile::ref rem_ref = 0; rem_ref < piles.size(); rem_ref++) {
        if ((rules.hole && rem_ref == hole) || piles[rem_ref].empty())
            continue;
        if (undoes_prev_move(rem_ref, parent_move))
            continue;

        if (rules.foundations && is_foundation(rem_ref)) {
            if (!rules.foundations_removable) continue;
            else if (dominance_blocks_foundation_move(rem_ref)) continue;
        }

        if (rules.stock_size > 0 && rem_ref == stock) {
            if (rules.stock_deal_t == sdt::WASTE)
                add_stock_to_waste_move(moves);
            continue;
        }

        if (rules.tableau_pile_count > 0)
            add_tableau_moves(moves, rem_ref);
        if (rules.cells > 0)
            add_cell_moves(moves, rem_ref);
        if (rules.foundations)
            add_foundation_moves(moves, rem_ref);
        if (rules.hole && is_valid_hole_move(rem_ref))
            moves.emplace_back(move::mtype::regular, rem_ref, hole);
    }

    if (rules.move_built_group)
        add_built_group_moves(moves);

    if (rules.foundations_comp_piles) // I.E. Spider-type winning condition
        add_foundation_complete_piles_moves(moves);

    return moves;
}


////////////////////////////////
// REGULAR MOVE GEN FUNCTIONS //
////////////////////////////////

bool game_state::stock_can_deal_tableau() const {
    return rules.stock_deal_t == sdt::TABLEAU_PILES
           && !piles[stock].empty();
}

// This is a special kind of move which the solver handles differently.
// We must supply the number of stock cards
// that we will deal, so that the move can be undone in backtracking.
move game_state::get_stock_tableau_move() const {
    pile::size_type stock_moves =
            piles[stock].size() >= tableau_piles.size()
            ? pile::size_type(tableau_piles.size())
            : piles[stock].size();

    assert(stock_moves > 0 && stock_moves <= piles[stock].size());
    return move(move::mtype::stock_to_tableau, 0, 0, stock_moves);
}

bool game_state::stock_can_redeal() const {
    return rules.stock_redeal
           && rules.stock_deal_t == sdt::WASTE
           && piles[stock].empty()
           && !piles[waste].empty();
}

bool game_state::undoes_prev_move(pile::ref rem_ref, move parent_move) const {
    return rem_ref == parent_move.to
           && parent_move.count == 1
           && parent_move.from != stock;
}

bool game_state::is_foundation(pile::ref ref) const {
    return ref >= foundations.front() && ref <= foundations.back();
}

void game_state::add_stock_to_waste_move(std::vector<move>& moves) const {
    uint8_t cards_to_move = min(rules.stock_deal_count, piles[stock].size());
    moves.emplace_back(move::mtype::stock_to_waste, stock, waste, cards_to_move);
}

void game_state::add_tableau_moves(std::vector<move>& moves, pile::ref rem_ref) const {
    for (auto add_ref : tableau_piles) {
        if (is_valid_tableau_move(rem_ref, add_ref)) {
            moves.emplace_back(move::mtype::regular, rem_ref, add_ref);
        }
    }
}

void game_state::add_cell_moves(std::vector<move>& moves, pile::ref rem_ref) const {
    for (auto add_ref : cells) {
        if (add_ref != rem_ref && piles[add_ref].empty()) {
            moves.emplace_back(move::mtype::regular, rem_ref, add_ref);
        }
    }
}

void game_state::add_foundation_moves(std::vector<move>& moves, pile::ref rem_ref) const {
    for (auto add_ref : foundations) {
        if (is_valid_foundations_move(rem_ref, add_ref)) {
            moves.emplace_back(move::mtype::regular, rem_ref, add_ref);
        }
    }
}

void game_state::add_foundation_complete_piles_moves(std::vector<move>& moves) const {
    for (pile::ref tab_pr : tableau_piles) {
        if (is_ordered_pile(tab_pr)) {

            for (pile::ref found_pr : foundations) {
                if (piles[found_pr].empty()) {
                    moves.emplace_back(move::mtype::built_group, tab_pr, found_pr, rules.max_rank);
                    break;
                }
            }
        }
    }
}

bool game_state::is_valid_tableau_move(const pile::ref rem_ref,
                                       const pile::ref add_ref) const {
    if (rem_ref == add_ref || rules.build_pol == pol::NO_BUILD)
        return false;

    if (piles[add_ref].empty()) {
        switch(rules.spaces_pol) {
            case sol_rules::spaces_policy::NO_BUILD:
                return false;
            case sol_rules::spaces_policy::KINGS:
                return piles[rem_ref].top_card().get_rank() == 13;
            default:
                return true;
        }
    } else {
        card rem_c = piles[rem_ref].top_card();
        card add_c = piles[add_ref].top_card();

        return is_next_tableau_card(add_c, rem_c);
    }
}

bool game_state::is_next_tableau_card(card a, card b) const {
    return is_next_legal_card(rules.build_pol, a, b);
}

bool game_state::is_valid_foundations_move(const pile::ref rem_ref,
                                           const pile::ref add_ref) const {
    if (rem_ref == add_ref || rules.foundations_comp_piles) return false;

    card rem_c = piles[rem_ref].top_card();

    // Checks same suit policy
    uint8_t suit_idx = (add_ref - foundations.front()) % uint8_t(4);
    if (rem_c.get_suit() != suit_idx)
        return false;

    // Checks rank
    if (piles[add_ref].empty())
        return rem_c.get_rank() == 1;
    else
        return rem_c.get_rank() == piles[add_ref].top_card().get_rank() + 1;
}

bool game_state::is_valid_hole_move(const pile::ref rem_ref) const {
    if (rem_ref == hole) return false;

    card::rank_t rank = piles[rem_ref].top_card().get_rank();
    card::rank_t hole_rank = piles[hole].top_card().get_rank();

    return rank + 1 == hole_rank
           || rank - 1 == hole_rank
           || (rank == rules.max_rank && hole_rank == 1)
           || (rank == 1 && hole_rank == rules.max_rank);
}


////////////////////////////////////
// BUILT-GROUP MOVE GEN FUNCTIONS //
////////////////////////////////////

void game_state::add_built_group_moves(vector<move> &moves) const {
    assert(rules.built_group_pol != pol::NO_BUILD);

    // Cycles through each pile to see if it contains a built group
    for (auto rem_ref : tableau_piles) {
        if (piles[rem_ref].size() < 2) continue;

        auto built_group_height = get_built_group_height(rem_ref);
        if (built_group_height == 1) continue;

        // We have found a built group. Cycles through each pile to see if it can be added
        for (auto add_ref : tableau_piles) {
            if (add_ref == rem_ref) continue;

            card bg_high = piles[rem_ref][built_group_height - 1];

            if (piles[add_ref].empty()) {
                if (rules.spaces_pol == s_pol::ANY || (rules.spaces_pol == s_pol::KINGS && bg_high.get_rank() == 13))
                    add_empty_built_group_moves(moves, rem_ref, add_ref, bg_high);
            } else {
                if (is_next_built_group_card(piles[add_ref].top_card(), bg_high))
                    moves.emplace_back(move::mtype::built_group, rem_ref, add_ref, built_group_height);
            }
        }
    }
}

// Finds the size of the built group at the top of a pile
pile::size_type game_state::get_built_group_height(pile::ref ref) const {
    pile::size_type i = 1;
    while (is_next_built_group_card(piles[ref][i], piles[ref][i-1]) && i < piles[ref].size())
        i++;
    return i;
}

// Loops through each possible built group move to an empty pile and adds it to the list
void game_state::add_empty_built_group_moves(vector<move>& moves, pile::ref rem_ref, pile::ref add_ref,
                                             card bg_high) const {
    pile::size_type card_idx = 1;
    do {
        moves.emplace_back(move::mtype::built_group, rem_ref, add_ref, static_cast<pile::size_type>(card_idx + 1));
    } while (piles[rem_ref][card_idx++] != bg_high);
}

bool game_state::is_next_built_group_card(card a, card b) const {
    return is_next_legal_card(rules.built_group_pol, a, b);
}

bool game_state::is_next_legal_card(pol p, card a, card b) const {
    // Checks build pol violations
    switch(p) {
        case sol_rules::build_policy::SAME_SUIT:
            if (b.get_suit() != a.get_suit()) return false;
            break;
        case sol_rules::build_policy::RED_BLACK:
            if (b.get_colour() == a.get_colour()) return false;
            break;
        default:;
    }
    // Checks rank
    return b.get_rank() + 1 == a.get_rank();
}
