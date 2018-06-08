//
// Created by thecharlesblake on 07/06/18.
//

#ifndef SOLVITAIRE_MOVE_H
#define SOLVITAIRE_MOVE_H

#include "pile.h"

struct move {
    move(pile::ref , pile::ref, pile::size_type = 1);
    bool is_dominance() const;
    friend bool operator==(const move&, const move&);

    const static pile::size_type dominance_flag;
    pile::ref from; pile::ref to; pile::size_type count;
};


#endif //SOLVITAIRE_MOVE_H
