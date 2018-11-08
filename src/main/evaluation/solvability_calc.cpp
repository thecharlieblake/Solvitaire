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
#include <future>
#include <iomanip>
#include <iostream>
#include <omp.h>

#include "solvability_calc.h"
#include "../solver/solver.h"
#include "binomial_ci.h"

using namespace std;
using boost::optional;

typedef command_line_helper::streamliner_opt cmd_sos;
typedef game_state::streamliner_options sos;
typedef std::chrono::milliseconds millisec;

///////////////////
// SETUP METHODS //
///////////////////

solvability_calc::solvability_calc(const sol_rules& r, uint64_t cache_capacity_) :
        rules(r), cache_capacity(cache_capacity_) {
}

//////////////////////
// PRINTING METHODS //
//////////////////////

void solvability_calc::print_general_info(const seed_results& seed_res) {
    pair<double, double> interval = binomial_ci::wilson(
            seed_res.solvable,
            seed_res.unsolvable,
            (seed_res.timed_out + seed_res.mem_limit));

    cout << interval.first * 100
         << ", "  << interval.second * 100
         << ", "  << seed_res.solvable
         << ", "  << seed_res.unsolvable
         << ", "  << seed_res.timed_out
         << ", "  << seed_res.mem_limit;
}

void solvability_calc::print_seed_info(seed_result seed_res) {
    auto& res = seed_res.second;

    solver::print_result_csv(res);
}

void solvability_calc::print_seeds_in_prog(std::set<int>& seeds_in_progress) {
    for(auto iter = begin(seeds_in_progress); iter != end(seeds_in_progress); ++iter) {
        cout << ", " << *iter;
    }

    cout << "\n";

    std::flush(clog);
    std::flush(cout);
}


/////////////////////
// SOLVING METHODS //
/////////////////////

void solvability_calc::calculate_solvability_percentage(uint64_t timeout_, int seed_count_, uint cores,
                                                        cmd_sos stream_opt_, const vector<int>& resume) {
    resume_seeds = vector<int>(begin(resume) + 3, end(resume));
    sort(begin(resume_seeds), end(resume_seeds));

    current_seed = resume_seeds.back();
    resume_seeds.pop_back();

    seed_res.solvable   = resume[0];
    seed_res.unsolvable = resume[1];
    seed_res.timed_out  = resume[2];

    timeout = millisec(timeout_);
    solver::print_header(timeout.count(), stream_opt_);
    seed_count = seed_count_;
    stream_opt = stream_opt_;

    vector<std::thread> threads(cores);

    for (uint c = 0; c < cores; c++)
        threads[c] = std::thread(solver_thread, this, c);

    for (uint c = 0; c < cores; c++)
        threads[c].join();
}

void solvability_calc::solver_thread(solvability_calc* sc, uint core) {
    int my_seed = sc->resume_seeds.size() > core ? sc->resume_seeds[core] : sc->current_seed++;

    while (my_seed < sc->seed_count) {
        sc->results_mutex.lock();
        sc->seeds_in_progress.insert(my_seed);
        sc->results_mutex.unlock();

        optional<seed_result> stream_res, no_stream_res, final_res;

        if (sc->stream_opt == cmd_sos::SMART) {
            stream_res = solve_seed(my_seed, (sc->timeout/10), sc->rules, sc->cache_capacity, sos::BOTH);

            switch (stream_res->second.sol_type) {
                case solver::result::type::UNSOLVABLE:
                case solver::result::type::TIMEOUT:
                    no_stream_res = solve_seed(my_seed, sc->timeout, sc->rules, sc->cache_capacity, sos::NONE);
                    final_res = *no_stream_res;
                    break;
                default:
                    final_res = *stream_res;
                    break;
            }
        } else {
            no_stream_res = solve_seed(my_seed, sc->timeout, sc->rules, sc->cache_capacity,
                                       command_line_helper::convert_streamliners(sc->stream_opt));
            final_res = *no_stream_res;
        }

        sc->results_mutex.lock();

        sc->seeds_in_progress.erase(my_seed);
        sc->seed_res.add_result(final_res->second.sol_type);

        cout << my_seed;
        //print_general_info(sc->seed_res);
        if (sc->stream_opt == cmd_sos::SMART) {
            print_seed_info(*stream_res);
            if (no_stream_res) {
                print_seed_info(*no_stream_res);
                cout << ", " << no_stream_res->second.sol_type;
            } else {
                solver::print_null_seed_info();
                cout << ", " << stream_res->second.sol_type;
            }
        } else {
            print_seed_info(*no_stream_res);
            cout << ", " << no_stream_res->second.sol_type;
        }
        //print_seeds_in_prog(sc->seeds_in_progress);
        cout << "\n";

        sc->results_mutex.unlock();

        my_seed = sc->current_seed++;
    }
}

solvability_calc::seed_result solvability_calc::solve_seed(int seed, millisec timeout, const sol_rules& rules,
                                                          uint64_t cache_capacity,
                                                          game_state::streamliner_options stream_opt) {
    game_state gs(rules, seed, stream_opt);
    solver sol(gs, cache_capacity);

    return seed_result(seed, sol.run(optional<std::chrono::milliseconds>(timeout)));
}


////////////////////////
// SEED RESULTS CLASS //
////////////////////////

solvability_calc::seed_results::seed_results() : solvable(0), unsolvable(0), timed_out(0), mem_limit(0) {
}

void solvability_calc::seed_results::add_result(solver::result::type t) {
    switch (t) {
        case solver::result::type::TIMEOUT:
            timed_out++;
            break;
        case solver::result::type::SOLVED:
            solvable++;
            break;
        case solver::result::type::UNSOLVABLE:
            unsolvable++;
            break;
        case solver::result::type ::MEM_LIMIT:
            mem_limit++;
            break;
        case solver::result::type ::TERMINATED:
            break;
    }
}
