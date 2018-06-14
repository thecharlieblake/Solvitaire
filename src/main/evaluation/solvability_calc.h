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

    void calculate_solvability_percentage(int timeout);

private:
    typedef std::chrono::milliseconds millisec;

    struct sol_result {
        enum class type { timeout, solved, unsolvable };

        int seed;
        type sol_type;
        millisec time;
        int unique_search_states;
    };

    struct seed_results {
        void add_result(sol_result::type);

        std::atomic<int> solvable;
        std::atomic<int> unsolvable;
        std::atomic<int> intractable;
        //millisec solvable_time;
        //millisec unsolvable_time;
    };

    // Printing methods
    void print_header(long) const;
    void print_row(const seed_results&, sol_result) const;

    // Solving methods
    sol_result solve_seed(int, millisec, seed_results&);

    // Calculation methods
    double sol_lower_bound(int, int, int) const;
    double sol_upper_bound(int, int, int) const;
    double sol_ci_size(int, int, int) const;
    static double agresti_coull(int, int, bool);
    static double agresti_coull_mean(int, int);
    static int rnd(double);

    const sol_rules& rules;
    const uint64_t cache_capacity;
};


#endif //SOLVITAIRE_SOLVABILITY_H
