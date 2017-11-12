//
// Created by thecharlesblake on 10/24/17.
//

#ifndef SOLVITAIRE_GAME_STATE_H
#define SOLVITAIRE_GAME_STATE_H

#include <vector>
#include <ostream>
#include <string>

#include <rapidjson/document.h>

#include "card.h"
#include "sol_rules.h"
#include "pile.h"

class game_state {
public:
    // Create a game state representation from a JSON doc
    game_state(const rapidjson::Document&);
    // Do the same from a 'rules' object
    game_state(int seed, const sol_rules&);

    std::vector<game_state> get_next_legal_states() const;
    bool is_solved() const;

    std::ostream& print(std::ostream&) const;

    friend bool operator==(const game_state&, const game_state&);
    friend std::ostream& operator<< (std::ostream&, const game_state&);

private:
    static std::vector<card> shuffled_deck(int, int);

    void print_header(std::ostream&, const char*) const;
    void print_piles(std::ostream&, const std::vector<pile>&) const;
    void print_pile(std::ostream&, const pile&) const;
    void print_top_of_piles(std::ostream&, const std::vector<pile>&) const;
    void print_top_of_pile(std::ostream&, const pile&) const;

    std::vector<pile> foundations;
    std::vector<pile> cells;
    std::vector<pile> tableau_piles;
    pile reserve;
    pile stock;
    pile waste;
    pile hole;
    sol_rules rules;
};


#endif //SOLVITAIRE_GAME_STATE_H
