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

///////////////////
// SETUP METHODS //
///////////////////

solvability_calc::solvability_calc(const sol_rules& r, uint64_t cache_capacity_) :
        rules(r), cache_capacity(cache_capacity_) {
}

//////////////////////
// PRINTING METHODS //
//////////////////////

void solvability_calc::print_header(long t) const {
    cerr << "Calculating solvability percentage...\n\n"
            "[Lower Bound% - Upper Bound%], Solvable/Unsolvable/Intractable Count | "
            "Solved seed: Solvable/Unsolvable/Intractable, Time Taken (ms), Unique Search States | "
            "Seeds In Progress"
            "\n--- Timeout = " << t << " milliseconds ---";
    cerr << fixed << setprecision(3);
}

void solvability_calc::print_row(const seed_results& seed_res, sol_result res, set<int>& seeds_in_progress,
                                 mutex& sip_mutex) {
    double lower_bound = sol_lower_bound(seed_res.solvable,
                                         seed_res.unsolvable,
                                         seed_res.intractable);
    double upper_bound = sol_upper_bound(seed_res.solvable,
                                         seed_res.unsolvable,
                                         seed_res.intractable);

    cerr << "\n"
         << "["     << lower_bound * 100
         << " - "  << upper_bound * 100
         << "],\t"  << seed_res.solvable
         << "/"     << seed_res.unsolvable
         << "/"     << seed_res.intractable
         << ",\t"    << res.seed
         << ": ";
    switch (res.sol_type) {
        case sol_result::type::timeout:
            cerr << "timed-out,";
            break;
        case sol_result::type::solved:
            cerr << "solved,\t";
            break;
        case sol_result::type::unsolvable:
            cerr << "unsolvable,";
            break;
    }
    cerr << "\t" << res.time.count()
            << ", " << res.unique_search_states
            << " | [";

    sip_mutex.lock();
    for(auto iter = begin(seeds_in_progress); iter != end(seeds_in_progress); ++iter) {
        cerr << *iter;
        if (iter != prev(end(seeds_in_progress))) cerr << ",";
        else cerr << "]";
    }
    sip_mutex.unlock();
}


/////////////////////
// SOLVING METHODS //
/////////////////////

void solvability_calc::calculate_solvability_percentage(int timeout_) {
    atomic<int> current_seed(0);
    set<int> seeds_in_progress;
    mutex sip_mutex;

    int cores = 4;
    millisec timeout(timeout_);

    print_header(timeout.count());

    // Spin off 'cores' threads. Each one runs solve_seed and then print_row repeatedly, taking the seed from am
    // atomic int
    vector<future<void>> futures;
    seed_results seed_res;

    sol_rules sr = rules;
    uint64_t cc = cache_capacity;

    for (int i = 0; i < cores; i++) {
        futures.push_back(async(
                launch::async,
                [&current_seed, &seed_res, &seeds_in_progress, &sip_mutex, timeout, sr, cc](){

                    int my_seed = 0;
                    while (my_seed < INT_MAX) {
                        my_seed = current_seed++;

                        sip_mutex.lock();
                        seeds_in_progress.insert(my_seed);
                        sip_mutex.unlock();

                        sol_result res = solve_seed(my_seed, timeout, sr, cc, seed_res);
                        print_row(seed_res, res, seeds_in_progress, sip_mutex);

                        sip_mutex.lock();
                        seeds_in_progress.erase(my_seed);
                        sip_mutex.unlock();
                    }
                }
        ));
    }
    futures[0].wait(); // Should wait indefinitely
}

solvability_calc::sol_result solvability_calc::solve_seed(int seed, millisec timeout,
                                                          const sol_rules& rules, uint64_t cache_capacity,
                                                          seed_results& seed_res) {
    game_state gs(rules, seed);
    solver sol(gs, cache_capacity);
    atomic<bool> terminate_solver(false);

    sol_result res = sol_result();
    res.seed = seed;

    future<tuple<bool, int>> future = async(
            launch::async,
            [&sol, &terminate_solver](){
                bool solved = sol.run(terminate_solver) == solver::sol_state::solved;
                return make_tuple(solved, sol.get_unique_states_searched());
            }
    );

    future_status status;
    do {
        auto start = chrono::steady_clock::now();
        status = future.wait_for(timeout);

        if (status == future_status::timeout) {
            terminate_solver = true;
        } else if (status == future_status::ready) {
            auto result = future.get();

            if (terminate_solver) {
                res.sol_type = sol_result::type::timeout;
                res.time = timeout;
            } else {
                auto end = chrono::steady_clock::now();
                millisec elapsed_millis =
                        chrono::duration_cast<chrono::milliseconds>(end - start);
                res.time = elapsed_millis;

                if (get<0>(result)) {
                    //seed_res.solvable_time += elapsed_millis;
                    res.sol_type = sol_result::type::solved;
                } else {
                    //seed_res.unsolvable_time += elapsed_millis;
                    res.sol_type = sol_result::type::unsolvable;
                }
            }
            seed_res.add_result(res.sol_type);

            res.unique_search_states = get<1>(result);
        }
    } while (status != future_status::ready);

    return res;
}


////////////////////////
// SEED RESULTS CLASS //
////////////////////////

solvability_calc::seed_results::seed_results() : solvable(0), unsolvable(0), intractable(0) {
}

void solvability_calc::seed_results::add_result(sol_result::type t) {
    switch (t) {
        case sol_result::type::timeout:
            intractable++;
            break;
        case sol_result::type::solved:
            solvable++;
            break;
        case sol_result::type::unsolvable:
            unsolvable++;
            break;
    }
}


/////////////////////////
// CALCULATION METHODS //
/////////////////////////

double solvability_calc::sol_lower_bound(int solvables,
                                         int unsolvables,
                                         int intractables) {
    int x = solvables;
    int n = solvables + unsolvables + intractables;
    bool lower_bound = true;
    return agresti_coull(x, n, lower_bound);
}

double solvability_calc::sol_upper_bound(int solvables,
                                         int unsolvables,
                                         int intractables) {
    int x = solvables + intractables;
    int n = solvables + unsolvables + intractables;
    bool lower_bound = false;
    return agresti_coull(x, n, lower_bound);
}

double solvability_calc::sol_ci_size(int solvables,
                                     int unsolvables,
                                     int intractables) {
    return sol_upper_bound(solvables, unsolvables, intractables)
           - sol_lower_bound(solvables, unsolvables, intractables);
}

double solvability_calc::agresti_coull(int x, int n, bool lower_bound) {
    double z = 1.96;
    double n_ = n + pow(z, 2);
    double p = (x + pow(z, 2)/2)/n_;
    double half_ci = z * sqrt((p - pow(p, 2))/n_);
    if (lower_bound) {
        double ans = p - half_ci;
        return ans < 0 ? 0 : ans;
    } else {
        double ans = p + half_ci;
        return ans > 1 ? 1 : ans;
    }
}

double solvability_calc::agresti_coull_mean(int x, int n) {
    double z = 1.96;
    double n_ = n + pow(z, 2);
    double p = (x + pow(z, 2)/2)/n_;
    return p;
}

int solvability_calc::rnd(double d) { return static_cast<int>(lround(d)); }
