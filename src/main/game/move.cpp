//
// Created by thecharlesblake on 07/06/18.
//

#include "move.h"

move::move(mtype ty, pile::ref f, pile::ref t, int8_t count_, bool reveal_move_, bool flip_waste_) :
        type(ty), from(f), to(t), count(count_), reveal_move(reveal_move_), flip_waste(flip_waste_) {
#ifndef NDEBUG
    if (ty == mtype::regular)      assert(count == 1);
    if (ty == mtype::stock_k_plus) assert(!reveal_move_);
    if (ty != mtype::stock_k_plus) assert(count >= 1 && !flip_waste_);
#endif
}

void move::make_reveal_move() {
    reveal_move = true;
}

bool operator==(const move& a, const move& b) {
    return    (a.from     == b.from )
              && (a.to    == b.to   )
              && (a.count == b.count)
              && (a.type  == b.type )
              && (a.reveal_move == b.reveal_move);
}
