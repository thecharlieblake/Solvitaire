//
// Created by thecharlesblake on 10/24/17.
//

#ifndef SOLVITAIRE_GAME_STATE_H
#define SOLVITAIRE_GAME_STATE_H

#define NULL_MOVE (game_state::move(255, 255))

#include <vector>
#include <ostream>
#include <string>

#include <rapidjson/document.h>
#include <boost/functional/hash.hpp>

#include "card.h"
#include "sol_rules.h"
#include "pile.h"

class deal_parser;

class game_state {
    friend class deal_parser;
public:
    typedef uint8_t pile_ref;
    typedef std::pair<pile_ref, pile_ref> move;

    // Create a game state representation from a JSON doc
    game_state(const sol_rules&, const rapidjson::Document&);
    // Do the same from a 'rules' object
    game_state(const sol_rules&, int seed);

    void make_move(move);
    void undo_move(move);
    std::vector<move> get_legal_moves() const;
    bool is_solved() const;
    const std::vector<pile>& get_data() const;

    std::ostream& print(std::ostream&) const;

    friend bool operator==(const game_state&, const game_state&);
    friend std::ostream& operator<< (std::ostream&, const game_state&);
    friend std::size_t hash_value(game_state const&);
    friend std::size_t hash_value(std::vector<pile> const&);

private:
    static std::vector<card> gen_shuffled_deck(int, int);

    explicit game_state(const sol_rules&);

    void print_header(std::ostream&, const char*) const;
    void print_piles(std::ostream&, const std::vector<pile_ref>&) const;
    void print_pile(std::ostream&, pile_ref) const;
    void print_top_of_piles(std::ostream&, const std::vector<pile_ref>&) const;
    void print_top_of_pile(std::ostream&, pile_ref) const;

    sol_rules rules;
    std::vector<pile_ref> foundations;
    std::vector<pile_ref> cells;
    std::vector<pile_ref> tableau_piles;
    pile_ref reserve;
    pile_ref stock;
    pile_ref waste;
    pile_ref hole;

    std::vector<pile_ref> addable_piles;
    std::vector<pile_ref> removable_piles;

    std::vector<pile> piles;
};


#endif //SOLVITAIRE_GAME_STATE_H
