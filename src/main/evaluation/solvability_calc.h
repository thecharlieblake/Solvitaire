//
// Created by thecharlesblake on 3/21/18.
//

#ifndef SOLVITAIRE_SOLVABILITY_H
#define SOLVITAIRE_SOLVABILITY_H

#include <vector>

#include "../game/sol_rules.h"

class solvability_calc {
public:
    explicit solvability_calc(const sol_rules&, uint64_t);

    void calculate_solvability_percentage();

private:
    typedef std::chrono::milliseconds millisec;

    struct seed_results {
        seed_results();
        int solvables() const;
        int unsolvables() const;
        int intractables() const;
        void add_intractable_results(const seed_results&);

        std::vector<int> solvable;
        std::vector<int> unsolvable;
        std::vector<int> intractable;
        millisec solvable_time;
        millisec unsolvable_time;
    };

    struct inc_t_coeffs {
        inc_t_coeffs(double, double, double);
        double solvable;    // Expected solvable    if we double the timeout
        double unsolvable;  // Expected unsolvable  if we double the timeout
        double intractable; // Expected intractable if we double the timeout
        double time;        // Expected time taken  if we double the timeout (as
                            // fraction of timeout)
        void update(const seed_results&, millisec);
    };

    // Printing methods
    void print_header(long) const;
    void print_row(const seed_results&, int, double, double) const;
    void print_begin_re_eval_msg(long) const;
    void print_end_re_eval_msg() const;

    // Solving methods
    void solve_seed(int, millisec, seed_results&);
    double continue_ci_size(inc_t_coeffs, const seed_results &, millisec) const;
    double inc_timeout_ci_size(inc_t_coeffs, const seed_results &) const;

    // Calculation methods
    double sol_lower_bound(int, int, int) const;
    double sol_upper_bound(int, int, int) const;
    double sol_ci_size(int, int, int) const;
    static double agresti_coull(int, int, bool);
    static double agresti_coull_mean(int, int);
    static int rnd(double);

    const sol_rules& rules;
    const static double coeff_ewma_weight;
    const uint64_t cache_capacity;
};


#endif //SOLVITAIRE_SOLVABILITY_H
