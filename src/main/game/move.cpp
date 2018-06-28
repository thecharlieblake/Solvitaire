//
// Created by thecharlesblake on 07/06/18.
//

#include "move.h"

move::move(mtype ty, pile::ref f, pile::ref t, pile::size_type i, bool reveal_move_) :
        type(ty), from(f), to(t), count(i), reveal_move(reveal_move_) {
#ifndef NDEBUG
    if (ty == mtype::regular) assert(i == 1);
    else if (ty != mtype::stock_to_waste) assert(i >= 1);
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
