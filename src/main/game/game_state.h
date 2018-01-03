//
// Created by thecharlesblake on 10/24/17.
//

#ifndef SOLVITAIRE_GAME_STATE_H
#define SOLVITAIRE_GAME_STATE_H

#define NULL_MOVE (game_state::move(PILE_REF_MAX, PILE_REF_MAX))
#define PILE_REF_MAX (255)

#include <vector>
#include <ostream>
#include <string>

#include <rapidjson/document.h>
#include <boost/functional/hash.hpp>

#include "card.h"
#include "pile.h"
#include "sol_rules.h"

class deal_parser;

class game_state {
    friend class deal_parser;
public:
    // Defines types
    typedef uint8_t pile_ref;
    typedef std::pair<pile_ref, pile_ref> move;

    // Constructors
    // Creates a game state representation from a JSON doc
    game_state(const sol_rules&, const rapidjson::Document&);
    // Does the same from a 'rules' object
    game_state(const sol_rules&, int seed);

    // Alters state
    void make_move(move);
    void undo_move(move);

    std::vector<move> get_legal_moves() const;

    // Inspects state
    bool is_solved() const;
    const std::vector<pile>& get_data() const;

    // Prints
    std::ostream& print(std::ostream&) const;

    friend bool operator==(const game_state&, const game_state&);
    friend std::ostream& operator<< (std::ostream&, const game_state&);
    friend std::size_t hash_value(game_state const&);
    friend std::size_t hash_value(std::vector<pile> const&);

private:
    static std::vector<card> gen_shuffled_deck(int, int);

    // Private constructor
    explicit game_state(const sol_rules&);

    // Used in get_legal_moves()
    bool is_valid_tableau_move(pile_ref, pile_ref) const;
    bool is_valid_foundations_move(pile_ref, pile_ref) const;
    bool is_valid_hole_move(pile_ref) const;

    // Prints
    void print_header(std::ostream&, const char*) const;
    void print_piles(std::ostream&, const std::vector<pile_ref>&) const;
    void print_pile(std::ostream&, pile_ref) const;
    void print_top_of_piles(std::ostream&, const std::vector<pile_ref>&) const;
    void print_top_of_pile(std::ostream&, pile_ref) const;

    // References to piles
    sol_rules rules;
    std::vector<pile_ref> tableau_piles;
    std::vector<pile_ref> cells;
    pile_ref stock;
    pile_ref waste;
    std::vector<pile_ref> foundations;
    pile_ref hole;

    // The core piles
    std::vector<pile> piles;
};


#endif //SOLVITAIRE_GAME_STATE_H
