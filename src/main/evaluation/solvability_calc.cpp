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

const double solvability_calc::coeff_ewma_weight = 0.7;

solvability_calc::solvability_calc(const sol_rules& r, uint64_t cache_capacity_) :
        rules(r), cache_capacity(cache_capacity_) {
}

//////////////////////
// PRINTING METHODS //
//////////////////////

void solvability_calc::print_header(long t) const {
    cerr << "Calculating solvability percentage...\n\n"
            "[Lower Bound, Upper Bound] | Solvable/Unsolvable/Intractable "
            "| Current seed | (Est. Confidence Interval: Continue/Re-eval)"
            "\n--- Timeout = " << t << " milliseconds ---";
    cerr << fixed << setprecision(3);
}

void solvability_calc::print_row(const seed_results& seed_res, int seed,
                                 double cont, double re_eval) const {
    double lower_bound = sol_lower_bound(seed_res.solvables(),
                                         seed_res.unsolvables(),
                                         seed_res.intractables());
    double upper_bound = sol_upper_bound(seed_res.solvables(),
                                         seed_res.unsolvables(),
                                         seed_res.intractables());

    if (cont != -1 && re_eval !=-1) {
        cerr << "\n"
             << "["    << lower_bound * 100
             << ", "   << upper_bound * 100
             << "] | " << seed_res.solvables()
             << "/"    << seed_res.unsolvables()
             << "/"    << seed_res.intractables()
             << " | "  << seed
             << " | (";
        if (seed_res.intractables() == 0) {
            cerr << "N/A";
        } else {
            cerr  << cont * 100
                  << ", " << re_eval * 100;
        }
        cerr << ")";
    } else {
        cerr << "\n"
             << "[-- re-eval --] | " << seed_res.solvables()
             << "/"    << seed_res.unsolvables()
             << "/"    << seed_res.intractables()
             << " | "  << seed;
    }
}

void solvability_calc::print_begin_re_eval_msg(long t) const {
    cerr << "\n--- Timeout doubled to " << t
         << " ms. Re-evaluating intractable seeds ---";
}

void solvability_calc::print_end_re_eval_msg() const {
    cerr << "\n--- Re-evaluation complete ---";
}


/////////////////////
// SOLVING METHODS //
/////////////////////

void solvability_calc::calculate_solvability_percentage() {
    int current_seed = 0;
    millisec timeout(500);
    millisec max_timeout(200000);
    inc_t_coeffs coeffs(0.4, 0.0, 1.0);

    print_header(timeout.count());

    seed_results seed_res;
    while(current_seed < INT_MAX) {

        double cont_ci_sz = continue_ci_size(coeffs, seed_res, timeout);
        double reeval_ci_sz = inc_timeout_ci_size(coeffs, seed_res);
        print_row(seed_res, current_seed, cont_ci_sz, reeval_ci_sz);

        while (seed_res.intractables() == 0
               || cont_ci_sz < reeval_ci_sz
               || timeout == max_timeout) {
            solve_seed(current_seed, timeout, seed_res);

            cont_ci_sz = continue_ci_size(coeffs, seed_res, timeout);
            reeval_ci_sz = inc_timeout_ci_size(coeffs, seed_res);

            print_row(seed_res, current_seed, cont_ci_sz, reeval_ci_sz);
            current_seed++;
        }

        timeout = min(timeout * 2, max_timeout);

        seed_results intract_seed_res;
        print_begin_re_eval_msg(timeout.count());
        for (auto intract_it  = begin(seed_res.intractable)
                ; intract_it != end  (seed_res.intractable)
                ; intract_it++) {
            solve_seed(*intract_it, timeout, intract_seed_res);
            print_row(intract_seed_res, *intract_it, -1, -1);
        }
        print_end_re_eval_msg();

        seed_res.add_intractable_results(intract_seed_res);
        coeffs.update(intract_seed_res, timeout);
    }
}

void solvability_calc::solve_seed(int seed, millisec timeout,
                                          seed_results& seed_res) {
    game_state gs(rules, seed);
    solver sol(gs, cache_capacity);
    atomic<bool> terminate_solver(false);

    future<bool> future = std::async(
            launch::async,
            [&sol, &terminate_solver](){
                return sol.run(terminate_solver) == solver::sol_state::solved;
            }
    );

    future_status status;
    do {
        auto start = chrono::steady_clock::now();
        status = future.wait_for(timeout);

        if (status == future_status::timeout) {
            terminate_solver = true;
        } else if (status == future_status::ready) {

            if (terminate_solver) {
                seed_res.intractable.push_back(seed);
            } else {
                auto end = chrono::steady_clock::now();
                millisec elapsed_millis =
                        chrono::duration_cast<chrono::milliseconds>(end - start);

                if (future.get()) {
                    seed_res.solvable.push_back(seed);
                    seed_res.solvable_time += elapsed_millis;
                } else {
                    seed_res.unsolvable.push_back(seed);
                    seed_res.unsolvable_time += elapsed_millis;
                }
            }
        }
    } while (status != future_status::ready);
}

double solvability_calc::continue_ci_size(inc_t_coeffs coeffs,
                                          const seed_results &seed_res,
                                          millisec timeout) const {
    // If there are no intractable seeds yet, returns 0 (i.e. min interval),
    // as continuing is only option
    if (seed_res.intractables() == 0) return 0;

    // Calculates the expected time to solve the intractable seeds with the
    // timeout doubled
    millisec inc_t_time(
            rnd(seed_res.intractables() * coeffs.time * 2 * timeout.count())
    );

    // Calculates the expected number of seeds we would have attempted to solve
    // were we to continue our search (i.e. without doubling timeout) in the
    // above time period
    millisec time_so_far = seed_res.solvable_time
                           + seed_res.unsolvable_time
                           + seed_res.intractables() * timeout;
    int total_seeds = seed_res.solvables() + seed_res.unsolvables() + seed_res.intractables();
    auto total_seeds_after = static_cast<int>(total_seeds * (1 + inc_t_time / time_so_far));

    // Use what we know so far about the solvable/unsolvable instances to
    // estimate the mean solvable/unsolvable
    double mean_solvable    = agresti_coull_mean(seed_res.solvables()   , total_seeds);
    double mean_unsolvable  = agresti_coull_mean(seed_res.unsolvables() , total_seeds);
    double mean_intractable = agresti_coull_mean(seed_res.intractables(), total_seeds);

    // Normalise
    double mean_totals = mean_solvable + mean_unsolvable + mean_intractable;
    mean_solvable /= mean_totals; mean_unsolvable /= mean_totals; mean_intractable /= mean_totals;

    // Calculates the expected number of solvable/unsolvable/intractable if we continue
    // for the length of time that would have been used to solve the intractable
    // seeds with the timeout doubled
    int total_solvable    = rnd(mean_solvable    * total_seeds_after);
    int total_unsolvable  = rnd(mean_unsolvable  * total_seeds_after);
    int total_intractable = rnd(mean_intractable * total_seeds_after);

    // Given the above hypothetical values, calculates the size of the
    // confidence interval we would be left with and returns it
    return sol_ci_size(total_solvable, total_unsolvable, total_intractable);
}

double solvability_calc::inc_timeout_ci_size(inc_t_coeffs coeffs,
                                             const seed_results &seed_res)
const {
    // Calculates the expected number of solvable/unsolvable/intractable
    // if we run the solver on the intractable instances with double the timeout
    int expected_solvable    = rnd(coeffs.solvable    * seed_res.intractables());
    int expected_unsolvable  = rnd(coeffs.unsolvable  * seed_res.intractables());
    int expected_intractable = rnd(coeffs.intractable * seed_res.intractables());

    // Given the above values, calculates the total number of
    // solvable/unsolvable/intractable we would expect to have afterwards
    int total_solvable    = seed_res.solvables()   + expected_solvable   ;
    int total_unsolvable  = seed_res.unsolvables() + expected_unsolvable ;
    int total_intractable =                          expected_intractable;

    // Given the above hypothetical values, calculates the size of the
    // confidence interval we would be left with and returns it
    return sol_ci_size(total_solvable, total_unsolvable, total_intractable);
}


////////////////////////
// SEED RESULTS CLASS //
////////////////////////

solvability_calc::seed_results::seed_results()
        : solvable_time(0), unsolvable_time(0) {
}

int solvability_calc::seed_results::solvables() const {
    return static_cast<int>(solvable.size());
}

int solvability_calc::seed_results::unsolvables() const {
    return static_cast<int>(unsolvable.size());
}

int solvability_calc::seed_results::intractables() const {
    return static_cast<int>(intractable.size());
}

void solvability_calc::seed_results::add_intractable_results(
        const seed_results &ir) {
    solvable  .insert(end(solvable)  , begin(ir.solvable)  , end(ir.solvable  ));
    unsolvable.insert(end(unsolvable), begin(ir.unsolvable), end(ir.unsolvable));
    solvable_time   += ir.solvable_time  ;
    unsolvable_time += ir.unsolvable_time;

    intractable = ir.intractable;
}

solvability_calc::inc_t_coeffs::inc_t_coeffs(double solv, double intract,
                                             double ti)
        : solvable(solv), unsolvable(1-solv-intract), intractable(intract),
          time(ti) {
}

// Updates the coefficients that represent the expected results of running
// the solver on the intractable seeds with a doubled timeout. These updates
// consist of calculating exponentially-wieghted moving averages.
void solvability_calc::inc_t_coeffs::update(const seed_results &res,
                                            solvability_calc::millisec timeout) {
    int total_seeds = res.solvables() + res.unsolvables() + res.intractables();

    double mean_solvable    = agresti_coull_mean(res.solvables()   , total_seeds);
    double mean_unsolvable  = agresti_coull_mean(res.unsolvables() , total_seeds);
    double mean_intractable = agresti_coull_mean(res.intractables(), total_seeds);

    // Normalise
    double mean_totals = mean_solvable + mean_unsolvable + mean_intractable;
    mean_solvable /= mean_totals; mean_unsolvable /= mean_totals; mean_intractable /= mean_totals;

    // The time taken if all intractable instances were still intractable with
    // the doubled timeout
    millisec all_timeout_time = timeout * total_seeds;
    millisec total_time = res.solvable_time
                        + res.unsolvable_time
                        + res.intractables() * timeout;
    double time_frac = total_time / all_timeout_time;

    double w = coeff_ewma_weight, w_ = 1 - w;
    solvable    = w * mean_solvable    + w_ * solvable   ;
    unsolvable  = w * mean_unsolvable  + w_ * unsolvable ;
    intractable = w * mean_intractable + w_ * intractable;
    time        = w * time_frac        + w_ * time       ;
}


/////////////////////////
// CALCULATION METHODS //
/////////////////////////

double solvability_calc::sol_lower_bound(int solvables,
                                         int unsolvables,
                                         int intractables) const {
    int x = solvables;
    int n = solvables + unsolvables + intractables;
    bool lower_bound = true;
    return agresti_coull(x, n, lower_bound);
}

double solvability_calc::sol_upper_bound(int solvables,
                                         int unsolvables,
                                         int intractables) const {
    int x = solvables + intractables;
    int n = solvables + unsolvables + intractables;
    bool lower_bound = false;
    return agresti_coull(x, n, lower_bound);
}

double solvability_calc::sol_ci_size(int solvables,
                                     int unsolvables,
                                     int intractables) const {
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
