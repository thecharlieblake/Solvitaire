//
// Created by thecharlesblake on 3/21/18.
//

#ifndef SOLVITAIRE_SOLVABILITY_H
#define SOLVITAIRE_SOLVABILITY_H

#include <vector>
#include <set>
#include <chrono>
#include <mutex>

#include "../game/sol_rules.h"
#include "../solver/solver.h"

class solvability_calc {
public:
    explicit solvability_calc(const sol_rules&, uint64_t);

    void calculate_solvability_percentage(uint64_t, int, uint, bool, const std::vector<int>&);

private:
    typedef std::chrono::milliseconds millisec;

    struct sol_result {
        enum class type { timeout, solved, unsolvable };

        int seed;
        type sol_type;
        millisec time;
        solver::solution_info sol_info;
    };

    struct seed_results {
        explicit seed_results(std::vector<int>);
        void add_result(sol_result::type);

        std::atomic<int> solvable;
        std::atomic<int> unsolvable;
        std::atomic<int> intractable;
        //millisec solvable_time;
        //millisec unsolvable_time;
    };

    // Printing methods
    void print_header(long, bool) const;
    static void print_row(const seed_results&, sol_result, boost::optional<sol_result::type>, std::set<int>&);

    // Solving methods
    static sol_result solve_seed(int, millisec, const sol_rules&, uint64_t, bool);

    const sol_rules& rules;
    const uint64_t cache_capacity;
};


#endif //SOLVITAIRE_SOLVABILITY_H
