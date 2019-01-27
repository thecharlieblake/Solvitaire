/*
  Solvitaire: a solver for perfect information solitaire games
  Copyright (C) 2018 Charles Blake <thecharlesblake@live.co.uk> and 
  Ian Gent <Ian.Gent@st-andrews.ac.uk>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program (see LICENSE file); if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
//
// Created by thecharlesblake on 10/24/17.
//

#ifndef SOLVITAIRE_GAME_STATE_H
#define SOLVITAIRE_GAME_STATE_H

#include <vector>
#include <list>
#include <string>
#include <random>
#include <functional>

#include <boost/functional/hash.hpp>
#include <boost/optional/optional.hpp>

#include "document.h"
#include "../card.h"
#include "../pile.h"
#include "../sol_rules.h"
#include "../move.h"

class game_state {
    friend class hasher;
    friend class global_cache;
    friend class cached_game_state;
    friend class deal_parser;
    friend class state_printer;
    friend class test_helper;
    friend class json_helper;
public:
    enum class streamliner_options {NONE, AUTO_FOUNDATIONS, SUIT_SYMMETRY, BOTH};

    /* Constructors */

    // Creates a game state representation from a JSON doc
    game_state(const sol_rules&, const rapidjson::Document&, streamliner_options);
    // Does the same from a seed
    game_state(const sol_rules&, int seed, streamliner_options);
    // Does the same but with an initialiser list (useful for testing)
    game_state(const sol_rules&, std::initializer_list<std::initializer_list<std::string>>);

    /* Altering state */

    void make_move(move);
    void undo_move(move);
    void place_card(pile::ref, card);
    card take_card(pile::ref);

    /* Legal move generation */

    std::vector<move> get_legal_moves(move = move(move::mtype::regular));
    boost::optional<move> get_dominance_move() const;

    /* State inspection */

    bool is_solved() const;
    const std::vector<pile>& get_data() const;

    /* Printing */

    friend std::ostream& operator<< (std::ostream&, const game_state&);

private:
    /* Constructors (& helper function) */

    explicit game_state(const sol_rules&, streamliner_options);
    static std::vector<card> gen_shuffled_deck(card::rank_t, bool, std::mt19937);
    template<class RandomIt, class URBG> static void shuffle(RandomIt, RandomIt, URBG&&);

    /* Pile order logic */

    void eval_pile_order(pile::ref, bool);
    void eval_pile_order(std::list<pile::ref>&, pile::ref, bool);

    /* Altering state */

    void make_regular_move(move move);
    void undo_regular_move(move move);
    void make_built_group_move(move move);
    void undo_built_group_move(move move);
    void make_stock_k_plus_move(move move);
    void undo_stock_k_plus_move(move move);
    void make_stock_to_all_tableau_move(move move);
    void undo_stock_to_all_tableau_move(move move);
    void make_sequence_move(move move);
    void undo_sequence_move(move move);
    void make_accordion_move(move move);
    void undo_accordion_move(move move);

#ifndef NDEBUG
    void check_face_down_consistent() const;
#endif

    /* Legal move generation */

    bool stock_can_deal_all_tableau() const;
    move get_stock_to_all_tableau_move() const;

    std::set<std::pair<int8_t, bool>, std::greater<>> generate_k_plus_moves_to_check() const;
    void add_stock_to_cell_move(std::vector<move>&, pile::ref) const;
    void add_stock_to_tableau_moves(std::vector<move>&) const;
    void add_stock_to_hole_foundation_moves(std::vector<move>&) const;
    card stock_card_from_count(int8_t) const;
    void add_foundation_complete_piles_moves(std::vector<move> &) const;
    void add_accordion_moves(std::vector<move>&) const;
    void add_stock_hole_move(std::vector<move>&) const;

    bool is_valid_tableau_move(pile::ref, pile::ref) const;
    bool is_valid_tableau_move(card, pile::ref) const;
    bool is_next_tableau_card(card, card) const;
    bool is_valid_foundations_move(pile::ref, pile::ref) const;
    bool is_valid_foundations_move(card, pile::ref) const;
    bool is_valid_hole_move(pile::ref) const;
    bool is_valid_hole_move(card) const;

    void add_valid_tableau_moves(std::vector<move>&, pile::ref) const;
    void add_built_group_moves(std::vector<move>&, bool, bool) const;
    void add_built_group_moves(std::vector<move>&, pile::ref, pile::size_type, bool, bool) const;
    void add_whole_pile_moves(std::vector<move>&) const;
    void add_whole_pile_moves(std::vector<move>&, pile::ref, pile::size_type) const;
    pile::size_type get_built_group_height(pile::ref) const;
    bool is_next_built_group_card(card, card) const;
    void add_empty_built_group_moves(std::vector<move>&, pile::ref, pile::ref, pile::size_type, bool, bool, bool) const;
    void add_kings_only_built_group_move(std::vector<move>&, pile::ref, pile::ref, pile::size_type, bool) const;
    void add_non_empty_built_group_move(std::vector<move>&, pile::ref, pile::ref, pile::size_type, bool, bool, bool) const;
    void add_sequence_moves(std::vector<move>&) const;
    bool creates_immediate_loop(pile::ref, pile::ref) const;
    bool tableau_space_and_auto_reserve() const;

    bool is_next_legal_card(sol_rules::build_policy, card, card) const;
    bool is_next_legal_card(std::vector<sol_rules::accordion_policy>, card, card) const;
    void turn_face_down_cards(std::vector<move>&) const;

    /* Auto-foundation moves */

    boost::optional<move> auto_reserve_move() const;
    boost::optional<move> auto_waste_stock_move() const;
    bool is_valid_auto_foundation_move(pile::ref) const;
    bool is_ordered_pile(pile::ref) const;
    bool dominance_blocks_foundation_move(pile::ref);
    card::rank_t foundation_base_convert(card::rank_t) const;

    /* Game rules */

    const sol_rules rules;
    streamliner_options stream_opts;
    card::rank_t foundations_base;

    /* Pile references */

    std::list<pile::ref> tableau_piles;
    std::list<pile::ref> cells;
    pile::ref stock;
    pile::ref waste;
    std::list<pile::ref> reserve;
    std::vector<pile::ref> foundations;
    std::vector<pile::ref> sequences;
    std::list<pile::ref> accordion;
    pile::ref hole;

    /* Pile references of starting/original layout */

    std::vector<pile::ref> original_tableau_piles;
    std::vector<pile::ref> original_cells;
    std::vector<pile::ref> original_reserve;

    /* Core piles */

    std::vector<pile> piles;
};

#endif //SOLVITAIRE_GAME_STATE_H
