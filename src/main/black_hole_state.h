//
// Created by thecharlesblake on 10/23/17.
//

#ifndef SOLVITAIRE_BLACK_HOLE_STATE_H
#define SOLVITAIRE_BLACK_HOLE_STATE_H

#include <vector>
#include <ostream>

#include <rapidjson/document.h>

#include "card.h
#include "game_state.h"

class black_hole_state : public game_state {
public:
    black_hole_state(const rapidjson::Document& doc);
    std::vector<game_state> get_next_legal_states() const override;
    bool is_solved() const override;
    std::ostream& print(std::ostream&) const override;

    friend std::ostream& operator<< (std::ostream&, const black_hole_state&);

protected:
    virtual static const int MAX_RANK = 13;

private:
    void move_to_hole(int);
    static bool adjacent(const card& a, const card& b);

    card hole_card;
    mutable bool solved;
};

#endif //SOLVITAIRE_BLACK_HOLE_STATE_H
