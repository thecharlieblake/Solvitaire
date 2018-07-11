//
// Created by thecharlesblake on 07/06/18.
//

#ifndef SOLVITAIRE_MOVE_H
#define SOLVITAIRE_MOVE_H

#include "pile.h"

struct move {
    enum class mtype : uint8_t {regular, dominance, built_group, stock_k_plus, stock_to_all_tableau, sequence, null};

    explicit move(mtype, pile::ref = 255 , pile::ref = 255, int8_t = 1, bool reveal_move_ = false, bool flip_waste_ = false);
    friend bool operator==(const move&, const move&);

    void make_reveal_move();

    mtype type;
    pile::ref from;
    pile::ref to;
    // For built piles the size of the pile, or for stock k-plus moves,
    // the number of cards to be moved from the stock to the waste before the waste card is played on the foundations
    int8_t count;
    bool reveal_move;
    bool flip_waste;
};

#endif //SOLVITAIRE_MOVE_H
