//
// Created by thecharlesblake on 10/24/17.
//

#ifndef SOLVITAIRE_GAME_STATE_H
#define SOLVITAIRE_GAME_STATE_H

#include <vector>
#include <list>
#include <string>

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
public:
    /* Constructors */

    // Creates a game state representation from a JSON doc
    game_state(const sol_rules&, const rapidjson::Document&);
    // Does the same from a seed
    game_state(const sol_rules&, int seed);
    // Does the same but with an initialiser list (useful for testing)
    game_state(const sol_rules&, std::initializer_list<pile>);

    /* Altering state */

    void make_move(move);
    void undo_move(move);
    void place_card(pile::ref, card);
    card take_card(pile::ref);

    /* Legal move generation */

    std::vector<move> get_legal_moves(move = move(255, 255, 255));
    boost::optional<move> get_dominance_move() const;

    /* State inspection */

    bool is_solved() const;
    const std::vector<pile>& get_data() const;

    /* Printing */

    friend std::ostream& operator<< (std::ostream&, const game_state&);

private:
    /* Constructors (& helper function) */

    explicit game_state(const sol_rules&);
    static std::vector<card> gen_shuffled_deck(int, card::rank_t, bool);

    /* Pile order logic */

    void eval_pile_order(pile::ref, bool);
    void eval_pile_order(std::list<pile::ref>&, pile::ref, bool);

    /* Legal move generation */

    move get_stock_tableau_move() const;
    bool is_valid_tableau_move(pile::ref, pile::ref) const;
    bool is_valid_foundations_move(pile::ref, pile::ref) const;
    bool is_valid_hole_move(pile::ref) const;
    void get_built_group_moves(std::vector<move>&) const;
    pile::size_type get_built_group_height(pile::ref) const;
    bool valid_built_group_move(card, card, card) const;
    void add_empty_built_group_moves(std::vector<move>&, pile::ref, pile::ref,
                                     card) const;
    bool dominance_blocks_foundation_move(pile::ref);
    bool is_ordered_pile(pile::ref) const;

    /* Auto-foundation moves */

    void update_auto_foundation_moves(pile::ref);
    bool is_valid_auto_foundation_move(pile::ref) const;


    /* Game rules */

    const sol_rules rules;

    /* Pile references */

    std::list<pile::ref> tableau_piles;
    std::list<pile::ref> cells;
    pile::ref stock;
    pile::ref waste;
    std::list<pile::ref> reserve;
    std::vector<pile::ref> foundations;
    pile::ref hole;

    /* Pile references of starting/original layout */

    std::vector<pile::ref> original_tableau_piles;
    std::vector<pile::ref> original_cells;
    std::vector<pile::ref> original_reserve;

    /* Auto-foundation moves state */

    std::vector<bool> auto_foundation_moves;

    /* Core piles */

    std::vector<pile> piles;
};

#endif //SOLVITAIRE_GAME_STATE_H
