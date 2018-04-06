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

#include "../../../lib/rapidjson/document.h"
#include "card.h"
#include "pile.h"
#include "sol_rules.h"

class game_state {
    friend class deal_parser;
    friend class state_printer;
    friend class predicate;
    friend class hasher;
    friend class global_cache;
    friend class cached_game_state;
public:
    // Defines types
    typedef uint8_t pile_ref;
    struct move {
        move(pile_ref, pile_ref, pile::size_type = 1);
        bool is_dominance() const;
        friend bool operator==(const move&, const move&);

        const static pile::size_type dominance_flag;
        pile_ref from; pile_ref to; pile::size_type count;
    };

    // Constructors
    // Creates a game state representation from a JSON doc
    game_state(const sol_rules&, const rapidjson::Document&);
    // Does the same from a seed
    game_state(const sol_rules&, int seed);
    // Does the same but with an initialiser list (useful for testing)
    game_state(const sol_rules&, std::initializer_list<pile>);

    // Alters state
    void make_move(move);
    void undo_move(move);
    void place_card(pile_ref, card);
    card take_card(pile_ref);

    std::vector<move> get_legal_moves(
#ifndef NO_CARD_JUST_MOVED
            move = move(255, 255, 255)
#endif
    );

    // Inspects state
    bool is_solved() const;
    const std::vector<pile>& get_data() const;
    boost::optional<move> get_dominance_move() const;

    friend std::ostream& operator<< (std::ostream&, const game_state&);

private:
    static std::vector<card> gen_shuffled_deck(int, card::rank_t, bool);

    // Private constructor
    explicit game_state(const sol_rules&);

    // Used to maintain pile order invariant
    void eval_pile_order(pile_ref, bool);
    void eval_pile_order(std::list<pile_ref>&, pile_ref, bool);

    // Used in get_legal_moves()
    move get_stock_tableau_move() const;
    bool is_valid_tableau_move(pile_ref, pile_ref) const;
    bool is_valid_foundations_move(pile_ref, pile_ref) const;
    bool is_valid_hole_move(pile_ref) const;
    void get_built_group_moves(std::vector<move>&) const;
    pile::size_type get_built_group_height(pile_ref) const;
    bool valid_built_group_move(card, card, card) const;
    void add_built_group_move(std::vector<move>&, pile_ref, pile_ref) const;
    void add_empty_built_group_moves(std::vector<move>&, pile_ref, pile_ref,
                                     card) const;
    bool dominance_blocks_foundation_move(pile_ref);
    bool is_ordered_pile(pile_ref) const;

    void update_auto_foundation_moves(pile_ref);
    bool is_valid_auto_foundation_move(pile_ref) const;

    // References to piles
    const sol_rules rules;
    std::list<pile_ref> tableau_piles;
    std::list<pile_ref> cells;
    pile_ref stock;
    pile_ref waste;
    std::list<pile_ref> reserve;
    std::vector<pile_ref> foundations;
    pile_ref hole;

    // Stores the original pile refs of the commutative piles so that they can
    // be printed in the order the user expects
    std::vector<pile_ref> original_tableau_piles;
    std::vector<pile_ref> original_cells;
    std::vector<pile_ref> original_reserve;

    std::vector<bool> auto_foundation_moves;

    // The core piles
    std::vector<pile> piles;
};

#endif //SOLVITAIRE_GAME_STATE_H
