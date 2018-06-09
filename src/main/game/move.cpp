//
// Created by thecharlesblake on 07/06/18.
//

#include "move.h"

move::move(mtype ty, pile::ref f, pile::ref t, pile::size_type i) : type(ty), from(f), to(t), count(i) {
#ifndef NDEBUG
    if (ty == mtype::regular) assert(i == 1);
    else assert(i >= 1);
#endif
}

bool operator==(const move& a, const move& b) {
    return    (a.from     == b.from )
              && (a.to    == b.to   )
              && (a.count == b.count)
              && (a.type  == b.type );
}
