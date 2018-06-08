//
// Created by thecharlesblake on 07/06/18.
//

#include "move.h"

const pile::size_type move::dominance_flag = pile::max_size_type;

move::move(pile::ref f, pile::ref t, pile::size_type i) : from(f), to(t), count(i) {
    assert(i >= 1);
}

// Returns true is the move has been flagged as a special 'dominance' move
bool move::is_dominance() const {
    return count == dominance_flag;
}

bool operator==(const move& a, const move& b) {
    return    (a.from  == b.from )
              && (a.to    == b.to   )
              && (a.count == b.count);
}