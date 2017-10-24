//
// Created by thecharlesblake on 10/23/17.
//

#ifndef SOLVITAIRE_GAME_STATE_H
#define SOLVITAIRE_GAME_STATE_H

#include <vector>
#include <ostream>

#include <rapidjson/document.h>

#include "card.h

class game_state {
public:
    game_state(const rapidjson::Document&);
    virtual std::vector<game_state> get_next_legal_states() const = 0;
    virtual bool is_solved() const = 0;
    virtual std::ostream& print(std::ostream&) const;

    friend std::ostream& operator<< (std::ostream&, const game_state&);

protected:
    std::vector<std::vector<card>> tableau_piles;
};

#endif //SOLVITAIRE_GAME_STATE_H
