//
// Created by thecharlesblake on 3/21/18.
//
#include <future>
#include <iomanip>
#include <iostream>
#include <vector>
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

void solvability_calc::print_header(long t, cmd_sos stream_opt) const {
    cout << "Calculating solvability percentage...\n\n"
            "Lower Bound"
            ", Upper Bound"
            ", Solvable"
            ", Unsolvable"
            ", Timed-out"
            ", Memory Limited, ";
    if (stream_opt == cmd_sos::SMART) {
        cout << "| (Streamliner Results:) "
                "Attempted Seed"
                ", Outcome"
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
                "| (Non-Streamliner Results:) ";
    }
    cout << "Attempted Seed"
            ", Outcome"
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
    int seed = seed_res.first;
    auto& res = seed_res.second;

    cout << ", " << seed
         << ", " << res.sol_type
         << ", " << res.time.count()
         << ", " << res.states_searched
         << ", " << res.unique_states_searched
         << ", " << res.backtracks
         << ", " << res.dominance_moves
         << ", " << res.states_removed_from_cache
         << ", " << res.cache_size
         << ", " << res.cache_bucket_count
         << ", " << res.max_depth
         << ", " << res.depth;
}

void solvability_calc::print_null_seed_info() {
    cout << ", , , , , , , , , , , , ";
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

void solvability_calc::calculate_solvability_percentage(uint64_t timeout_, int seed_count, uint cores,
                                                        cmd_sos stream_opt, const vector<int>& resume) {
    vector<int> resume_seeds(begin(resume) + 3, end(resume));
    sort(begin(resume_seeds), end(resume_seeds));

    atomic<int> current_seed(resume_seeds.back());
    resume_seeds.pop_back();

    set<int> seeds_in_progress;
    seed_results seed_res(resume);
    mutex results_mutex;

    millisec timeout(timeout_);
    print_header(timeout.count(), stream_opt);

    sol_rules sr = rules;
    uint64_t cc = cache_capacity;

#pragma omp parallel for
    for (uint i = 0; i < cores; i++) {

        int my_seed = resume_seeds.size() > i ? resume_seeds[i] : current_seed++;

        while (my_seed < seed_count) {
            results_mutex.lock();
            seeds_in_progress.insert(my_seed);
            results_mutex.unlock();

            optional<seed_result> stream_res, no_stream_res, final_res;

            if (stream_opt == cmd_sos::SMART) {
                stream_res = solve_seed(my_seed, (timeout / 10), sr, cc, sos::BOTH);

                switch (stream_res->second.sol_type) {
                    case solver::result::type::UNSOLVABLE:
                    case solver::result::type::TIMEOUT:
                        no_stream_res = solve_seed(my_seed, timeout, sr, cc, sos::NONE);
                        final_res = *no_stream_res;
                        break;
                    default:
                        final_res = *stream_res;
                        break;
                }
            } else {
                no_stream_res = solve_seed(my_seed, timeout, sr, cc,
                                           command_line_helper::convert_streamliners(stream_opt));
                final_res = *no_stream_res;
            }

            results_mutex.lock();

            seeds_in_progress.erase(my_seed);
            seed_res.add_result(final_res->second.sol_type);

            print_general_info(seed_res);
            if (stream_opt == cmd_sos::SMART) {
                print_seed_info(*stream_res);
                if (no_stream_res) print_seed_info(*no_stream_res);
                else print_null_seed_info();
            } else {
                print_seed_info(*no_stream_res);
            }
            print_seeds_in_prog(seeds_in_progress);

            results_mutex.unlock();

            my_seed = current_seed++;
        }
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

solvability_calc::seed_results::seed_results(vector<int> v) : solvable(v[0]), unsolvable(v[1]), timed_out(v[2]), mem_limit(0) {
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
    }
}
