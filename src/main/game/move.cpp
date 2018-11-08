/*
  Solvitaire: a solver for perfect information solitaire games
  Copyright (C) 2018 Charles Blake <thecharlesblake@live.co.uk> and 
  Ian Gent <Ian.Gent@st-andrews.ac.uk>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program (see LICENSE file); if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
//
// Created by thecharlesblake on 07/06/18.
//

#include "move.h"

move::move(mtype ty, pile::ref f, pile::ref t, int8_t count_, bool reveal_move_, bool flip_waste_, bool dominance_move_) :
        type(ty), from(f), to(t), count(count_), reveal_move(reveal_move_), flip_waste(flip_waste_), dominance_move(dominance_move_) { 
#ifndef NDEBUG
    if (ty == mtype::regular)      assert(count == 1);
    if (ty == mtype::stock_k_plus) assert(!reveal_move_);
#endif
}

void move::make_reveal_move() {
    reveal_move = true;
}

void move::make_dominance_move() {
    dominance_move = true;
}

bool operator==(const move& a, const move& b) {
    return    (a.from     == b.from )
              && (a.to    == b.to   )
              && (a.count == b.count)
              && (a.type  == b.type )
              && (a.reveal_move == b.reveal_move)
	      && (a.dominance_move == b.dominance_move) ;
}
