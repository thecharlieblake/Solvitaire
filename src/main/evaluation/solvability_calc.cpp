//
// Created by thecharlesblake on 3/21/18.
//
#include <future>
#include <iomanip>
#include <iostream>

#include "solvability_calc.h"
#include "../solver/solver.h"
#include "binomial_ci.h"

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
    cout << "Calculating solvability percentage...\n\n"
            "Lower Bound"
            ", Upper Bound"
            ", Solvable"
            ", Unsolvable"
            ", Intractable"
            ", Attempted Seed"
            ", Seed State"
            ", Time Taken(ms)"
            ", States Searched"
            ", Unique States Searched"
            ", Backtracks"
            ", Dominance Moves"
            ", States Removed From Cache"
            ", Final States In Cache"
            ", Final Buckets In Cache"
            ", Maximum Search Depth"
            ", Final Search Depth"
            ", Seeds In Progress ..."
            "\n--- Timeout = " << t << " milliseconds ---\n";
    cout << fixed << setprecision(3);
}

void solvability_calc::print_row(const seed_results& seed_res, sol_result res, set<int>& seeds_in_progress) {
    pair<double, double> interval = binomial_ci::wilson(
            seed_res.solvable,
            seed_res.unsolvable,
            seed_res.intractable);

    cout << interval.first * 100
         << ", "  << interval.second * 100
         << ", "  << seed_res.solvable
         << ", "  << seed_res.unsolvable
         << ", "  << seed_res.intractable
         << ", "  << res.seed;
    switch (res.sol_type) {
        case sol_result::type::timeout:
            cout << ", timed-out";
            break;
        case sol_result::type::solved:
            cout << ", solved";
            break;
        case sol_result::type::unsolvable:
            cout << ", unsolvable";
            break;
    }
    cout << ", " << res.time.count();
    
    cout << ", " << res.sol_info.states_searched
         << ", " << res.sol_info.unique_states_searched
         << ", " << res.sol_info.backtracks
         << ", " << res.sol_info.dominance_moves
         << ", " << res.sol_info.states_removed_from_cache
         << ", " << res.sol_info.cache_size
         << ", " << res.sol_info.cache_bucket_count
         << ", " << res.sol_info.max_depth
         << ", " << res.sol_info.depth;

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

void solvability_calc::calculate_solvability_percentage(uint64_t timeout_, int seed_count, uint cores, const vector<int>& resume) {
    vector<int> resume_seeds(begin(resume) + 3, end(resume));
    sort(begin(resume_seeds), end(resume_seeds));

    atomic<int> current_seed(resume_seeds.back());
    resume_seeds.pop_back();

    set<int> seeds_in_progress;
    seed_results seed_res(resume);
    mutex results_mutex;

    millisec timeout(timeout_);
    print_header(timeout.count());

    // Spin off 'cores' threads. Each one runs solve_seed and then print_row repeatedly, taking the seed from am
    // atomic int
    vector<future<void>> futures;

    sol_rules sr = rules;
    uint64_t cc = cache_capacity;

    for (uint i = 0; i < cores; i++) {
        futures.push_back(async(
                launch::async,
                [&current_seed, &seed_res, &seeds_in_progress, &results_mutex, seed_count, timeout, sr, cc, i,
                        resume_seeds](){

                    int my_seed = resume_seeds.size() > i ? resume_seeds[i] : current_seed++;

                    while (my_seed < seed_count) {
                        results_mutex.lock();
                        seeds_in_progress.insert(my_seed);
                        results_mutex.unlock();

                        sol_result res = solve_seed(my_seed, timeout, sr, cc);

                        results_mutex.lock();
                        seeds_in_progress.erase(my_seed);
                        switch (res.sol_type) {
                            case sol_result::type::solved:      seed_res.solvable   ++; break;
                            case sol_result::type::unsolvable:  seed_res.unsolvable ++; break;
                            case sol_result::type::timeout:     seed_res.intractable++; break;
                        }
                        print_row(seed_res, res, seeds_in_progress);
                        results_mutex.unlock();

                        my_seed = current_seed++;
                    }
                }
        ));
    }

    for (auto& f : futures) f.wait();
}

solvability_calc::sol_result solvability_calc::solve_seed(int seed, millisec timeout,
                                                          const sol_rules& rules, uint64_t cache_capacity) {
    game_state gs(rules, seed);
    solver sol(gs, cache_capacity);
    atomic<bool> terminate_solver(false);

    sol_result res = sol_result();
    res.seed = seed;

    future<tuple<bool, solver::solution_info>> future = async(
            launch::async,
            [&sol, &terminate_solver](){
                bool solved = sol.run(terminate_solver) == solver::sol_state::solved;
                return make_tuple(solved, sol.get_solution_info());
            }
    );

    future_status status;
    do {
        auto start = chrono::steady_clock::now();
        status = future.wait_for(timeout);

        if (status == future_status::timeout) {
            terminate_solver = true;
        } else if (status == future_status::ready) {
            bool exception = false;
            tuple<bool, solver::solution_info> result;

            try {
                result = future.get();
            } catch(runtime_error& e) {
                exception = true;
            }

            if (exception) {
                res.sol_type = sol_result::type::timeout;
                res.time = timeout;
                res.sol_info = get<1>(result);
            } else if (terminate_solver) {
                res.sol_type = sol_result::type::timeout;
                res.time = timeout;
                res.sol_info = get<1>(result);
            } else {
                auto end = chrono::steady_clock::now();
                millisec elapsed_millis =
                        chrono::duration_cast<chrono::milliseconds>(end - start);
                res.time = elapsed_millis;

                if (get<0>(result)) {
                    res.sol_type = sol_result::type::solved;
                } else {
                    res.sol_type = sol_result::type::unsolvable;
                }
                res.sol_info = get<1>(result);
            }
        }
    } while (status != future_status::ready);

    return res;
}


////////////////////////
// SEED RESULTS CLASS //
////////////////////////

solvability_calc::seed_results::seed_results(vector<int> v) : solvable(v[0]), unsolvable(v[1]), intractable(v[2]) {
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
