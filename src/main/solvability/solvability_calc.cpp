//
// Created by thecharlesblake on 3/21/18.
//
#include <future>
#include <iomanip>
#include <iostream>
#include <cmath>

#include "solvability_calc.h"
#include "../solver/solver.h"

using namespace std;


solvability_calc::solvability_calc(const sol_rules& r) : rules(r) {
}

void solvability_calc::calculate_solvability_percentage() const {
    cout << "Calculating solvability percentage...\n\n"
            "[Lower Bound, Upper Bound] | (Solvable/Unsolvable/Intractable) "
            "| Current seed\n";
    cout << fixed << setprecision(3);

    int solvable = 0;
    int unsolvable = 0;
    int intractable = 0;

    for(int seed = 0; seed < INT_MAX; seed++) {
        game_state gs(rules, seed);
        solver sol(gs);
        atomic<bool> terminate_solver(false);

        future<bool> future = std::async(launch::async,
                                         [&sol, &terminate_solver](){
                                             return sol.run(terminate_solver) == solver::sol_state::solved;
                                         });

        future_status status;
        do {
            status = future.wait_for(chrono::seconds(45));

            if (status == future_status::timeout) {
                terminate_solver = true;
            } else if (status == future_status::ready) {
                if (terminate_solver) {
                    intractable++;
                } else if (future.get()) {
                    solvable++;
                } else {
                    unsolvable++;
                }
            }
        } while (status != future_status::ready);

        double lower_bound = sol_lower_bound(solvable, unsolvable, intractable);
        double upper_bound = sol_upper_bound(solvable, unsolvable, intractable);

        cout << "[" << lower_bound * 100 << ", " << upper_bound * 100
             << "] | ("<< solvable << "/" << unsolvable << "/" << intractable
             << ") | " << seed << "\n";
    }
}

double solvability_calc::sol_lower_bound(int solvable, int unsolvable, int intractable) {
    int n = solvable + unsolvable + intractable;
    int x = solvable;
    double z = 1.96;
    bool lower_bound = true;
    return agrestiCoull(n, x, z, lower_bound);
}

double solvability_calc::sol_upper_bound(int solvable, int unsolvable, int intractable) {
    int n = solvable + unsolvable + intractable;
    int x = solvable + intractable;
    double z = 1.96;
    bool lower_bound = false;
    return agrestiCoull(n, x, z, lower_bound);
}

double solvability_calc::agrestiCoull(int n, int x, double z, bool lower_bound) {
    double n_ = n + pow(z, 2);
    double p = (x + pow(z, 2)/2)/n_;
    double v = z * sqrt((p - pow(p, 2))/n_);
    if (lower_bound) {
        double ans = p - v;
        return ans < 0 ? 0 : ans;
    } else {
        double ans = p + v;
        return ans > 1 ? 1 : ans;
    }
}