//
// Created by thecharlesblake on 07/06/18.
//

#include "game_state.h"
#include "../sol_rules.h"
#include "../move.h"

#include <boost/optional/optional.hpp>
#include <iostream>

using std::vector;
using std::min;
using std::list;
using std::pair;
using std::set;
using std::greater;
using namespace rapidjson;

typedef sol_rules::build_policy pol;
typedef sol_rules::spaces_policy s_pol;
typedef sol_rules::accordion_policy acc_pol;
typedef sol_rules::stock_deal_type sdt;
typedef sol_rules::face_up_policy fu;
typedef sol_rules::direction dir;
typedef sol_rules::built_group_type bgt;

////////////////////////////////
// MAIN LEGAL MOVE GEN CYCLE ///
////////////////////////////////

// Note that the moves added last here, are tried first
vector<move> game_state::get_legal_moves(move parent_move) {
    // Order:
    // Accordion moves
    // Foundations complete piles moves
    // Tableau / cells / reserve / stock-waste (redeal) to hole / foundation moves
    // Cell to tableau moves
    // Sequence to sequence moves
    // Tableau to tableau moves
    // Tableau built group moves
    // Stock-waste to tableau moves (redeal)
    // Reserve to tableau moves
    // Foundation to tableau moves
    // Stock-waste to tableau moves (no redeal)
    // Stock-waste (no redeal) to hole / foundation moves
    // Tableau / reserve / stock-waste to cell moves
    // Stock to all tableau moves

    vector<move> moves;

    // Stock to all tableau moves
    if (rules.stock_size > 0 && stock_can_deal_all_tableau())
            moves.emplace_back(get_stock_to_all_tableau_move());

    // Tableau / reserve / stock-waste to cell moves
    pile::ref empty_cell = 255;
    for (auto c : cells) {
        if (piles[c].empty()) empty_cell = c;
    }
    if (empty_cell != 255) {
        for (auto t : tableau_piles) {
            if (piles[t].empty() || parent_move.to == t) continue;
            else moves.emplace_back(move::mtype::regular, t, empty_cell);
        }

        for (auto r : reserve)
            if (!piles[r].empty())
                moves.emplace_back(move::mtype::regular, r, empty_cell);

        if (rules.stock_size > 0 && rules.stock_deal_t == sdt::WASTE)
            add_stock_to_cell_move(moves, empty_cell);
    }

    // Stock-waste (no redeal) to hole / foundation moves
    if (rules.hole || (rules.foundations_present && !rules.foundations_only_comp_piles)) {
        if (rules.stock_size > 0 && rules.stock_deal_t == sdt::WASTE && !rules.stock_redeal)
            add_stock_to_hole_foundation_moves(moves);
    }

    // Stock-waste to tableau moves (no redeal)
    if (rules.stock_size > 0 && rules.stock_deal_t == sdt::WASTE && !rules.stock_redeal)
        add_stock_to_tableau_moves(moves);

    // Foundation to tableau / empty cell moves
    if (rules.foundations_removable) {
        for (auto f : foundations) {
            if (piles[f].empty() || parent_move.to == f || dominance_blocks_foundation_move(f)) continue;

            add_valid_tableau_moves(moves, f);

            if (empty_cell != 255)
                moves.emplace_back(move::mtype::regular, f, empty_cell);
        }
    }

    // Reserve to tableau moves
    for (auto r : reserve) {
        if (piles[r].empty()) continue;

        add_valid_tableau_moves(moves, r);
    }

    // Stock-waste to tableau moves (redeal)
    if (rules.stock_size > 0 && rules.stock_deal_t == sdt::WASTE && rules.stock_redeal)
        add_stock_to_tableau_moves(moves);

    // Tableau built group moves
    switch (rules.move_built_group) {
        case sol_rules::built_group_type::YES:
            add_built_group_moves(moves, false);
            break;
        case sol_rules::built_group_type::MAXIMAL_GROUP:
            add_built_group_moves(moves, true);
            break;
        case sol_rules::built_group_type::WHOLE_PILE:
            add_whole_pile_moves(moves);
            break;
        case sol_rules::built_group_type::NO:
            break;
    }

    // Tableau to tableau moves
    // If only whole pile moves are available, doesn't make regular ones
//
//     if (rules.move_built_group != bgt::WHOLE_PILE && (rules.move_built_group != bgt::MAXIMAL_GROUP) 
    if ((rules.move_built_group != bgt::WHOLE_PILE) && (rules.move_built_group != bgt::MAXIMAL_GROUP)) {

//     if (rules.move_built_group != bgt::WHOLE_PILE) {
        for (auto t_from : tableau_piles) {
            // Forbids moves from empty piles, reversing parent moves, or from single-
            // card piles to empty piles
            if (piles[t_from].empty() || parent_move.to == t_from || tableau_space_and_auto_reserve()) continue;

            for (auto to : tableau_piles) {
                if (is_valid_tableau_move(t_from, to)
                    && !(piles[t_from].size() == 1 && piles[to].empty())) {
                    moves.emplace_back(move::mtype::regular, t_from, to);
                }
            }
        }
    }

    // Sequence to sequence moves
    if (rules.sequence_count > 0) {
        add_sequence_moves(moves);
    }

    // Cell to tableau moves
    for (auto c : cells) {
        if (piles[c].empty() || parent_move.to == c) continue;

        add_valid_tableau_moves(moves, c);
    }

    // Tableau / cells / reserve / stock-waste (redeal) to hole / foundation moves
    if (rules.hole || (rules.foundations_present && !rules.foundations_only_comp_piles)) {
        // Stock
        if (rules.stock_size > 0 && rules.stock_deal_t == sdt::WASTE && rules.stock_redeal)
            add_stock_to_hole_foundation_moves(moves);

        list<pile::ref> from_piles = tableau_piles;
        if (rules.cells > 0) from_piles.insert(from_piles.end(), cells.begin(), cells.end());
        if (rules.reserve_size > 0) from_piles.insert(from_piles.end(), reserve.begin(), reserve.end());

        for (auto fp : from_piles) {
            if (piles[fp].empty() || (parent_move.to == fp && parent_move.type != move::mtype::dominance)) continue;

            for (auto f : foundations)
                if (is_valid_foundations_move(fp, f))
                    moves.emplace_back(move::mtype::regular, fp, f);
            if (rules.hole && is_valid_hole_move(fp))
                moves.emplace_back(move::mtype::regular, fp, hole);
        }
    }

    if (rules.foundations_only_comp_piles) // i.e. Spider-type winning condition
        add_foundation_complete_piles_moves(moves);

    if (rules.tableau_pile_count > 0 && rules.face_up != fu::ALL)
        turn_face_down_cards(moves);

    if (rules.accordion_size > 0)
        add_accordion_moves(moves);

    return moves;
}


////////////////////////////////
// REGULAR MOVE GEN FUNCTIONS //
////////////////////////////////

bool game_state::stock_can_deal_all_tableau() const {
    return rules.stock_deal_t == sdt::TABLEAU_PILES
           && !piles[stock].empty();
}

// This is a special kind of move which the solver handles differently.
// We must supply the number of stock cards
// that we will deal, so that the move can be undone in backtracking.
move game_state::get_stock_to_all_tableau_move() const {
    pile::size_type stock_moves =
            piles[stock].size() >= tableau_piles.size()
            ? pile::size_type(tableau_piles.size())
            : piles[stock].size();

    assert(stock_moves > 0 && stock_moves <= piles[stock].size());
    return move(move::mtype::stock_to_all_tableau, 0, 0, stock_moves);
}

set<pair<int8_t, bool>, greater<>> game_state::generate_k_plus_moves_to_check() const {
    set<pair<int8_t, bool>, greater<>> stock_moves_to_check;

    // If the waste isn't empty, adds the move from the top of the current waste
    if (!piles[waste].empty()) stock_moves_to_check.insert(pair<int8_t, bool>(0, false));
    else if (piles[stock].empty()) return stock_moves_to_check;

    // Adds multiples of deal count (excluding last card)
    for (int8_t count = rules.stock_deal_count;
            count < piles[stock].size();
            count += rules.stock_deal_count)
        stock_moves_to_check.insert(pair<int8_t, bool>(count, false));

    // Adds last card
    bool flip_waste = rules.stock_redeal;
    stock_moves_to_check.insert(pair<int8_t, bool>(static_cast<int8_t>(piles[stock].size()), flip_waste));

    // If the stock can be redealt, searches through the waste then (if necessary) the stock again
    if (rules.stock_redeal) {
	// we do not need to go through waste and stock if the waste is a multiple of deal count 
	//
        if(piles[waste].size() % rules.stock_deal_count == 0) { 
            for (auto count = static_cast<int8_t>(-piles[waste].size() + rules.stock_deal_count);
                    count < 0;
                    count += rules.stock_deal_count)
                stock_moves_to_check.insert(pair<int8_t, bool>(count, false));
	}
	else {
            for (auto count = static_cast<int8_t>(-piles[waste].size() + rules.stock_deal_count);
                    count < piles[stock].size();
                    count += rules.stock_deal_count)
                stock_moves_to_check.insert(pair<int8_t, bool>(count, false));
	}
    }

    return stock_moves_to_check;
}

void game_state::add_stock_to_cell_move(std::vector<move>& moves, pile::ref empty_cell) const {
    for (auto k_plus_mv : generate_k_plus_moves_to_check())
        moves.emplace_back(move::mtype::stock_k_plus, stock, empty_cell, k_plus_mv.first, false, k_plus_mv.second);
}

void game_state::add_stock_to_tableau_moves(std::vector<move>& moves) const {
    // For each stock move to check, if it can be moved legally to one of the tableau
    // piles, adds this as a move
    for (auto k_plus_mv : generate_k_plus_moves_to_check()) {
        card from = stock_card_from_count(k_plus_mv.first);

        // Obeys the auto-reserve restriction unless the reserve is empty
        if (!piles[reserve.front()].empty() && tableau_space_and_auto_reserve()) return;

        for (auto t : tableau_piles) {
            if (is_valid_tableau_move(from, t)) {
                moves.emplace_back(move::mtype::stock_k_plus, stock, t, k_plus_mv.first, false, k_plus_mv.second);
            }
        }
    }
}

void game_state::add_stock_to_hole_foundation_moves(std::vector<move>& moves) const {
    for (auto k_plus_mv : generate_k_plus_moves_to_check()) {
        card from = stock_card_from_count(k_plus_mv.first);

        for (auto f : foundations)
            if (is_valid_foundations_move(from, f))
                moves.emplace_back(move::mtype::stock_k_plus, stock, f, k_plus_mv.first, false, k_plus_mv.second);

        if (rules.hole && is_valid_hole_move(from))
            moves.emplace_back(move::mtype::stock_k_plus, stock, hole, k_plus_mv.first, false, k_plus_mv.second);
    }
}

card game_state::stock_card_from_count(int8_t count) const {
    if (count > 0) {
        return piles[stock][count - 1];
    } else {
        return piles[waste][-count];
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

    return is_valid_tableau_move(piles[rem_ref].top_card(), add_ref);
}

bool game_state::is_valid_tableau_move(const card rem_c,
                                       const pile::ref add_ref) const {
    if (piles[add_ref].empty()) {
        switch(rules.spaces_pol) {
            case sol_rules::spaces_policy::NO_BUILD:
                return false;
            case sol_rules::spaces_policy::KINGS:
                return rem_c.get_rank() == 13;
            default:
                return true;
        }
    } else {
        card add_c = piles[add_ref].top_card();

        return is_next_tableau_card(add_c, rem_c);
    }
}

bool game_state::is_next_tableau_card(card a, card b) const {
    return is_next_legal_card(rules.build_pol, a, b);
}

bool game_state::is_valid_foundations_move(const pile::ref rem_ref,
                                           const pile::ref add_ref) const {
    if (rem_ref == add_ref || rules.foundations_only_comp_piles) return false;

    return is_valid_foundations_move(piles[rem_ref].top_card(), add_ref);
}

bool game_state::is_valid_foundations_move(const card rem_c,
                                           const pile::ref add_ref) const {
    if (piles[add_ref].size() == rules.max_rank) return false;

    // Checks same suit policy
    uint8_t suit_idx = (add_ref - foundations.front()) % uint8_t(4);
    if (rem_c.get_suit() != suit_idx)
        return false;

    // Checks rank
    if (piles[add_ref].empty())
        return rem_c.get_rank() == foundations_base;
    else
        return rem_c.get_rank() == (piles[add_ref].top_card().get_rank() % rules.max_rank) + 1;
}

bool game_state::is_valid_hole_move(const pile::ref rem_ref) const {
    if (rem_ref == hole) return false;
    return is_valid_hole_move(piles[rem_ref].top_card());
}

bool game_state::is_valid_hole_move(const card c) const {
    card::rank_t rank = c.get_rank();
    card::rank_t hole_rank = piles[hole].top_card().get_rank();

    return rank + 1 == hole_rank
           || rank - 1 == hole_rank
           || (rank == rules.max_rank && hole_rank == 1)
           || (rank == 1 && hole_rank == rules.max_rank);
}


////////////////////////////////////
// BUILT-GROUP MOVE GEN FUNCTIONS //
////////////////////////////////////

void game_state::add_valid_tableau_moves(std::vector<move>& moves, pile::ref from) const {
    if (tableau_space_and_auto_reserve()) return;

    for (auto to : tableau_piles) {
        if (is_valid_tableau_move(from, to)) {
            moves.emplace_back(move::mtype::regular, from, to);
        }
    }
}

void game_state::add_built_group_moves(vector<move>& moves, bool only_maximal) const {
    assert(rules.built_group_pol != pol::NO_BUILD);
    if (tableau_space_and_auto_reserve()) return;

    // Cycles through each pile to see if it contains a built group
    for (auto rem_ref : tableau_piles) {

	// if enforcing maximal piles then allow size one groups
        if (only_maximal) {
            if (piles[rem_ref].size() == 0) continue;
            auto built_group_height = get_built_group_height(rem_ref);
            add_built_group_moves(moves, rem_ref, built_group_height, only_maximal); 
        } 
	// otherwise size 1 groups are single cards and found elsewhere
        else { 
            if (piles[rem_ref].size() < 2) continue;
            auto built_group_height = get_built_group_height(rem_ref);
            if (built_group_height == 1) continue;
            add_built_group_moves(moves, rem_ref, built_group_height, only_maximal);
        }
    }
}

void game_state::add_built_group_moves(vector<move>& moves, pile::ref rem_ref, pile::size_type built_group_height,
                                       bool only_maximal) const {
    // We have found a built group. Cycles through each pile to see if it can be added
    for (auto add_ref : tableau_piles) {
        if (add_ref == rem_ref) continue;

        card bg_high = piles[rem_ref][built_group_height - 1];
        bool base_face_down =
                   piles[rem_ref].size() > built_group_height
                && piles[rem_ref][built_group_height].is_face_down();

        if (piles[add_ref].empty()) {
            if (rules.spaces_pol == s_pol::ANY) {
                add_empty_built_group_moves(moves, rem_ref, add_ref, built_group_height, base_face_down, only_maximal);
            } else if (rules.spaces_pol == s_pol::KINGS && bg_high.get_rank() == 13) {
                add_kings_only_built_group_move(moves, rem_ref, add_ref, built_group_height, base_face_down);
            }
        } else {
            add_non_empty_built_group_move(moves, rem_ref, add_ref, built_group_height, base_face_down, only_maximal);
        }
    }
}

void game_state::add_whole_pile_moves(vector<move>& moves) const {
    assert(rules.built_group_pol != pol::NO_BUILD);
    if (tableau_space_and_auto_reserve()) return;

    // Cycles through each pile to see if it contains a whole-pile built group
    for (auto rem_ref : tableau_piles) {
        auto built_group_height = get_built_group_height(rem_ref);

        if (built_group_height == piles[rem_ref].size()) {
            add_whole_pile_moves(moves, rem_ref, built_group_height);
        }
    }
}

void game_state::add_whole_pile_moves(vector<move>& moves, pile::ref rem_ref, pile::size_type built_group_height) const {
    // We have found a built group. Cycles through each pile to see if it can be added
    for (auto add_ref : tableau_piles) {
        if (add_ref == rem_ref) continue;

        card bg_high = piles[rem_ref][built_group_height - 1];

        if (piles[add_ref].empty()) {
            if (rules.spaces_pol == s_pol::ANY || (rules.spaces_pol == s_pol::KINGS && bg_high.get_rank() == 13)) {
                moves.emplace_back(move::mtype::built_group, rem_ref, add_ref, built_group_height);
            }
        } else if (is_next_built_group_card(piles[add_ref].top_card(),  bg_high)) {
            moves.emplace_back(move::mtype::built_group, rem_ref, add_ref, built_group_height);
        }
    }
}

// Finds the size of the built group at the top of a pile
pile::size_type game_state::get_built_group_height(pile::ref ref) const {
    pile::size_type i = 1;
    while (i < piles[ref].size()
           && is_next_built_group_card(piles[ref][i], piles[ref][i-1])
           && !piles[ref][i].is_face_down())
        i++;
    return i;
}

bool game_state::is_next_built_group_card(card a, card b) const {
    return is_next_legal_card(rules.built_group_pol, a, b);
}

// Loops through each possible built group move to an empty pile and adds it to the list
void game_state::add_empty_built_group_moves(vector<move>& moves, pile::ref rem_ref, pile::ref add_ref,
                                             pile::size_type built_group_height, bool base_face_down, bool only_maximal)
const {
    auto start_idx = static_cast<pile::size_type>(only_maximal ? built_group_height - 1 : 1);
    for (pile::size_type card_idx = start_idx; card_idx < built_group_height; card_idx++) {

        bool is_reveal_move = card_idx + 1 == built_group_height && base_face_down;
        moves.emplace_back(move::mtype::built_group, rem_ref, add_ref, card_idx + 1, is_reveal_move);
    }
}

void game_state::add_kings_only_built_group_move(vector<move>& moves, pile::ref rem_ref, pile::ref add_ref,
                                             pile::size_type built_group_height, bool base_face_down) const {

    moves.emplace_back(move::mtype::built_group, rem_ref, add_ref, built_group_height, base_face_down);
}

void game_state::add_non_empty_built_group_move(vector<move>& moves, pile::ref rem_ref, pile::ref add_ref,
                                             pile::size_type built_group_height, bool base_face_down, bool only_maximal) const {

    auto start_idx = static_cast<pile::size_type>(only_maximal ? built_group_height - 1 : 1);

    // Given rank of add pile, get card of that rank - 1 in rem pile and see if next bg card
    // For each card going down from top card in rem pile, check if is next bg card
    for (pile::ref r = start_idx; r < built_group_height; r++) {
        if (is_next_built_group_card(piles[add_ref].top_card(),  piles[rem_ref][r])) {

            bool is_reveal_move = r + 1 == built_group_height && base_face_down;
            moves.emplace_back(move::mtype::built_group, rem_ref, add_ref, r + 1, is_reveal_move);
            break;
        }
    }
}

void game_state::add_sequence_moves(std::vector<move>& moves) const {
    // Note, the sequence moves are encoded by counting each card's location (1-52) and adding that into 'to' and 'from'

    // Finds the spaces
    vector<pair<uint8_t, uint8_t>> space_locations;
    for (pile::ref i = 0; i < sequences.size(); i++) {

        for (pile::ref j = piles[sequences[i]].size(); j --> 0 ;) {
            if (piles[sequences[i]][j] == "AS") {
                space_locations.emplace_back(i, j);
            }
        }
    }

    // Searches through the cards again to find valid moves into the spaces
    for (pile::ref i = 0; i < sequences.size(); i++) {
        for (pile::ref j = piles[sequences[i]].size(); j --> 0 ;) {
            pile::ref from_idx = i * piles[sequences[i]].size() + j; // Encoded index of card to be moved
            card from_card = piles[sequences[i]][j];
            if (from_card == "AS") continue;

            // Adds valid moves
            for (pair<uint8_t, uint8_t> space_loc : space_locations) {
                pile::ref space_idx = space_loc.first * piles[sequences[i]].size() + space_loc.second; // Encoded index of space

                if (rules.sequence_direction == dir::LEFT || rules.sequence_direction == dir::BOTH) {
                    // Only twos in the far left column
                    if (space_loc.second == piles[sequences[i]].size() - 1) {
                        if (from_card.get_rank() == 2) {
                            bool is_legal = false;

                            if (rules.sequence_fixed_suit) {
                                if (from_card.get_suit() == space_loc.first) {
                                    is_legal = true;
                                }
                            } else {
                                is_legal = true;
                            }

                            if (is_legal) {
                                moves.emplace_back(move::mtype::sequence, from_idx, space_idx);
                            }
                        }
                    }
                    // Otherwise must agree with left neighbour
                    else {
                        card neighbour_card = piles[sequences[space_loc.first]][space_loc.second + 1];
                        if (neighbour_card != "AS" && is_next_legal_card(rules.sequence_build_pol, from_card, neighbour_card))
                            moves.emplace_back(move::mtype::sequence, from_idx, space_idx);
                    }
                }

                if (rules.sequence_direction == dir::RIGHT || rules.sequence_direction == dir::BOTH) {
                    // Only max rank in the far right column
                    if (space_loc.second == 0) {
                        if (from_card.get_rank() == rules.max_rank)
                            moves.emplace_back(move::mtype::sequence, from_idx, space_idx);
                    }
                    // Otherwise must agree with right neighbour
                    else {
                        card neighbour_card = piles[sequences[space_loc.first]][space_loc.second - 1];
                        if (neighbour_card != "AS" && is_next_legal_card(rules.sequence_build_pol, neighbour_card, from_card))
                            moves.emplace_back(move::mtype::sequence, from_idx, space_idx);
                    }
                }
            }
        }
    }
}

void game_state::add_accordion_moves(vector<move>& moves) const {
    int idx = 0, idx_max = static_cast<int>(accordion.size());
    for (auto from_it = begin(accordion); from_it != end(accordion); from_it++, idx++) {
        for (pair<dir, uint8_t> move_rule : rules.accordion_moves) {
            auto to_it = from_it;

            if        (move_rule.first == dir::LEFT  && idx - move_rule.second >= 0) {
                for (uint8_t i = 0; i < move_rule.second; i++) to_it--;
            } else if (move_rule.first == dir::RIGHT && idx + move_rule.second < idx_max) {
                for (uint8_t i = 0; i < move_rule.second; i++) to_it++;
            } else {
                continue;
            }

            if (is_next_legal_card(rules.accordion_pol, piles[*to_it].top_card(), piles[*from_it].top_card()))
                moves.emplace_back(move::mtype::accordion, *from_it, *to_it, piles[*from_it].size());
        }
    }
}

// If auto-reserve is enabled and there is a space, returns
bool game_state::tableau_space_and_auto_reserve() const {
    if (rules.spaces_pol == s_pol::AUTO_RESERVE_THEN_WASTE)
        for (auto to : tableau_piles)
            if (piles[to].empty()) return true;
    return false;
}

///////////////////////

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
    auto final_rank = foundations_base == 1
                      ? rules.max_rank
                      : foundations_base - card::rank_t(1);
    if (b.get_rank() == final_rank) {
        return false;
    } else {
        return (b.get_rank() % rules.max_rank) + 1 == a.get_rank();
    }
}

bool game_state::is_next_legal_card(vector<acc_pol> vp, card a, card b) const {
    for (auto p : vp) {
        switch (p) {
            case sol_rules::accordion_policy::SAME_RANK:
                if (b.get_rank() == a.get_rank()) return true;
                break;
            case sol_rules::accordion_policy::SAME_SUIT:
                if (b.get_suit() == a.get_suit()) return true;
                break;
            case sol_rules::accordion_policy::RED_BLACK:
                if (b.get_colour() != a.get_colour()) return true;
                break;
            case sol_rules::accordion_policy::ANY_SUIT:
                return true;
        }
    }
    return false;
}

void game_state::turn_face_down_cards(vector<move>& moves) const {
    for (auto& m : moves) {
        bool is_tableau_move = m.from >= original_tableau_piles.front() && m.from <= original_tableau_piles.back();
        if (is_tableau_move && piles[m.from].size() > 1 && piles[m.from][1].is_face_down()) {
            m.make_reveal_move();
        }
    }
}
