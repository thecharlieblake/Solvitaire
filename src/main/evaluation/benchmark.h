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
// Created by thecharlesblake on 4/5/18.
//

#ifndef SOLVITAIRE_BENCHMARK_H
#define SOLVITAIRE_BENCHMARK_H

#include <chrono>

#include "../game/sol_rules.h"
#include "../game/search-state/game_state.h"

class benchmark {
public:
    static void run(const sol_rules &rules, uint64_t, game_state::streamliner_options);
};


#endif //SOLVITAIRE_BENCHMARK_H
