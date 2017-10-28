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

class game_state {
public:
    game_state(const rapidjson::Document&, std::string);
    game_state(int seed, std::string);

    std::vector<game_state> get_next_legal_states() const;
    bool is_solved() const;
    std::ostream& print(std::ostream&) const;

    friend std::ostream& operator<< (std::ostream&, const game_state&);

private:
    static std::vector<card> shuffled_deck(int, int);

    void move_to_hole(int);
    bool adjacent(const card& a, const card& b) const;

    std::vector<std::vector<card> > tableau_piles;
    int max_rank;
    card hole_card;
    mutable bool solved;
};


#endif //SOLVITAIRE_GAME_STATE_H
