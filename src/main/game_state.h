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

class game_state {
public:
    game_state(const rapidjson::Document&);
    game_state(int seed, const sol_rules&);

    std::vector<game_state> get_next_legal_states() const;
    bool is_solved() const;
    std::ostream& print(std::ostream&) const;

    friend std::ostream& operator<< (std::ostream&, const game_state&);

private:
    static std::vector<card> shuffled_deck(int, int);

    void move_to_hole(int);
    bool adjacent(const card& a, const card& b) const;
    void print_header(std::ostream&, const char*) const;
    void print_foundations(std::ostream&) const;
    void print_tableau_piles(std::ostream&) const;
    void print_hole(std::ostream&) const;

    std::vector<std::vector<card> > tableau_piles;
    std::vector<card> foundations;
    sol_rules rules;
    card hole_card;
    mutable bool solved;
};


#endif //SOLVITAIRE_GAME_STATE_H
