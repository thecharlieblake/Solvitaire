//
// Created by thecharlesblake on 4/5/18.
//

#include <climits>
#include <iostream>

#include "benchmark.h"
#include "../game/game_state.h"
#include "../solver/solver.h"

using namespace std;
typedef chrono::microseconds microsec;

void benchmark::run(const sol_rules &rules) {
    cout << "Seed "
          "| Median Solution Time(μs) "
          "| Median States Searched "
          "| Median Backtracks "
          "| Median (Non-Zero) Solution Depth "
          "| Solvable/Unsolvable ";

    multiset<int> sol_times;
    multiset<int> states_searched;
    multiset<int> sol_depths;
    multiset<int> backtracks;
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
        states_searched.insert(sol.get_states_searched());
        if (sol.get_final_depth() != 0) sol_depths.insert(sol.get_final_depth());
        backtracks.insert(sol.get_backtrack_count());
        switch (result) {
            case solver::sol_state::solved:
                total_solvable++;
                break;
            case solver::sol_state::unsolvable:
                total_unsolvable++;
                break;
            case solver::sol_state::cutoff:
                assert(false);
                break;
        }

        cout << "\n"  << seed
             << " | " << *next(begin(sol_times), (sol_times.size())/2)
             << " | " << *next(begin(states_searched), (states_searched.size())/2)
             << " | " << *next(begin(backtracks), (backtracks.size())/2)
             << " | " << *next(begin(sol_depths), (sol_depths.size())/2)
             << " | " << total_solvable
             <<  "/"  << total_unsolvable;
    }
}
