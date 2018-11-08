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

#ifndef SOLVITAIRE_MOVE_H
#define SOLVITAIRE_MOVE_H

#include "pile.h"

struct move {
    enum class mtype : uint8_t {regular, built_group, stock_k_plus, stock_to_all_tableau, sequence, accordion, null};

    explicit move(mtype, pile::ref = 255 , pile::ref = 255, int8_t = 1, bool reveal_move_ = false, bool flip_waste_ = false, bool dominance_move_ = false);
    friend bool operator==(const move&, const move&);

    void make_reveal_move();
    void make_dominance_move();

    mtype type;
    pile::ref from;
    pile::ref to;
    // For built piles the size of the pile, or for stock k-plus moves,
    // the number of cards to be moved from the stock to the waste before the waste card is played on the foundations
    int8_t count;
    bool reveal_move;
    bool flip_waste;
    bool dominance_move;
};

#endif //SOLVITAIRE_MOVE_H
