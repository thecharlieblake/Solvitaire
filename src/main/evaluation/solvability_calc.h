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
// Created by thecharlesblake on 3/21/18.
//

#ifndef SOLVITAIRE_SOLVABILITY_H
#define SOLVITAIRE_SOLVABILITY_H

#include <vector>
#include <set>
#include <chrono>
#include <mutex>

#include "../game/sol_rules.h"
#include "../solver/solver.h"
#include "../input-output/input/command_line_helper.h"

class solvability_calc {
public:
    explicit solvability_calc(const sol_rules&, uint64_t);

    void calculate_solvability_percentage(uint64_t, int, uint, command_line_helper::streamliner_opt, const std::vector<int>&);

private:
    typedef std::pair<int, solver::result> seed_result;

    struct seed_results {
        seed_results();
        void add_result(solver::result::type);

        std::atomic<int> solvable;
        std::atomic<int> unsolvable;
        std::atomic<int> timed_out;
        std::atomic<int> mem_limit;
    };

    // Printing methods
    static void print_general_info(const seed_results&);
    static void print_seed_info(seed_result);
    static void print_seeds_in_prog(std::set<int>&);

    // Solving methods
    static void solver_thread(solvability_calc*, uint core);
    static seed_result solve_seed(int, std::chrono::milliseconds, const sol_rules&, uint64_t, game_state::streamliner_options);

    const sol_rules& rules;
    const uint64_t cache_capacity;
    std::chrono::milliseconds timeout;
    std::mutex results_mutex;
    seed_results seed_res;
    std::set<int> seeds_in_progress;
    std::vector<int> resume_seeds;
    std::atomic<int> current_seed;
    int seed_count;
    command_line_helper::streamliner_opt stream_opt;
};


#endif //SOLVITAIRE_SOLVABILITY_H
