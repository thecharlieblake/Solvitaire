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

void benchmark::run(const sol_rules &rules) {
    cout << "Seed "
            "| Median/Mean Solution Time(μs)"
            "| Median/Mean States Searched "
            "| Solvable/Unsolvable ";

    multiset<int> sol_times;
    multiset<int> states_searched;
    int total_time = 0;
    int total_states = 0;
    int total_solvable = 0;
    int total_unsolvable = 0;

    for(int seed = 1; seed <= 1000; seed++) {
        game_state gs(rules, seed);
        solver sol(gs);

        auto start = chrono::steady_clock::now();
        solver::sol_state result = sol.run();
        auto end = chrono::steady_clock::now();
        microsec elapsed_micros =
                chrono::duration_cast<chrono::microseconds>(end - start);

        sol_times.insert(static_cast<int>(elapsed_micros.count()));
        total_time += static_cast<int>(elapsed_micros.count());
        states_searched.insert(sol.get_states_searched());
        total_states += sol.get_states_searched();
        switch (result) {
            case solver::sol_state::solved:
                total_solvable++;
                break;
            case solver::sol_state::unsolvable:
                total_unsolvable++;
                break;
            case solver::sol_state::timed_out:
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
