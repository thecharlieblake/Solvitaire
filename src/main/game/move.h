//
// Created by thecharlesblake on 07/06/18.
//

#ifndef SOLVITAIRE_MOVE_H
#define SOLVITAIRE_MOVE_H

#include "pile.h"

struct move {
    enum class mtype : uint8_t {regular, dominance, built_group, stock_k_plus, stock_to_all_tableau, null};

    explicit move(mtype, pile::ref = 255 , pile::ref = 255, pile::size_type = 1, bool reveal_move_ = false);
    friend bool operator==(const move&, const move&);

    void make_reveal_move();

    mtype type;
    pile::ref from;
    pile::ref to;
    pile::size_type count;
    bool reveal_move;
};

#endif //SOLVITAIRE_MOVE_H
