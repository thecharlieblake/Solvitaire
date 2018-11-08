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

#include <climits>
#include <iostream>
#include <numeric>

#include "benchmark.h"
#include "../game/search-state/game_state.h"
#include "../solver/solver.h"

using namespace std;
typedef chrono::microseconds microsec;

void benchmark::run(const sol_rules &rules, uint64_t cache_capacity, game_state::streamliner_options streamliners) {
    cout << "Seed "
            "| Median/Mean Solution Time(μs) "
            "| Median/Mean States Searched "
            "| Solvable/Unsolvable";

    multiset<int> sol_times;
    multiset<int> states_searched;
    int total_time = 0;
    int total_states = 0;
    int total_solvable = 0;
    int total_unsolvable = 0;

    for(int seed = 1; seed <= 1000; seed++) {
        game_state gs(rules, seed, streamliners);
        solver sol(gs, cache_capacity);

        auto start = chrono::steady_clock::now();
        solver::result result = sol.run();
        auto end = chrono::steady_clock::now();
        microsec elapsed_micros =
                chrono::duration_cast<chrono::microseconds>(end - start);

        sol_times.insert(static_cast<int>(elapsed_micros.count()));
        total_time += static_cast<int>(elapsed_micros.count());
        states_searched.insert(result.states_searched);
        total_states += result.states_searched;
        switch (result.sol_type) {
            case solver::result::type::SOLVED:
                total_solvable++;
                break;
            case solver::result::type::UNSOLVABLE:
                total_unsolvable++;
                break;
            default:
                assert(false);
                break;
        }

        cout << "\n"  << seed
             << " | " << *next(begin(sol_times), (sol_times.size())/2)
             << "/" << total_time/sol_times.size()
             << " | " << *next(begin(states_searched), (states_searched.size())/2)
             << "/" << total_states/sol_times.size()
             << " | " << total_solvable
             <<  "/"  << total_unsolvable;
    }
}
