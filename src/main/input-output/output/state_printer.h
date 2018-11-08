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
// Created by thecharlesblake on 1/3/18.
//

#ifndef SOLVITAIRE_STATE_PRINT_H
#define SOLVITAIRE_STATE_PRINT_H

#include <ostream>

#include "../../game/search-state/game_state.h"

class state_printer {
public:
    static std::ostream& print(std::ostream&, const game_state&);
    static void print_move(std::ostream&, move);

private:
    static void print_header(std::ostream&, const char*);
    static void print_piles(std::ostream&,
                            const std::vector<pile::ref>&,
                            const game_state&);
    static void print_sequences(std::ostream&,
                            const std::vector<pile::ref>&,
                            const game_state&);
    static void print_top_of_piles(std::ostream&,
                                   const std::vector<pile::ref>&,
                                   const game_state&);
    static void print_top_of_pile(std::ostream&, pile::ref,
                                  const game_state&);
    static void print_accordion(std::ostream&, const std::list<pile::ref>&,
                                  const game_state&);
    static void print_card(std::ostream&, card);
};

#endif //SOLVITAIRE_STATE_PRINT_H
