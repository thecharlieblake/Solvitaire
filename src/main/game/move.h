//
// Created by thecharlesblake on 07/06/18.
//

#ifndef SOLVITAIRE_MOVE_H
#define SOLVITAIRE_MOVE_H

#include "pile.h"

struct move {
    enum class mtype {regular, dominance, built_group, stock_to_waste, stock_to_tableau, redeal, null};

    explicit move(mtype, pile::ref = 255 , pile::ref = 255, pile::size_type = 1);
    friend bool operator==(const move&, const move&);

    mtype type;
    pile::ref from;
    pile::ref to;
    pile::size_type count;
};


#endif //SOLVITAIRE_MOVE_H
