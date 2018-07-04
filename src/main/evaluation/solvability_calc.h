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
        enum class type { timeout, solved, unsolvable, mem_limit };

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
        std::atomic<int> timed_out;
        std::atomic<int> mem_limit;
    };

    // Printing methods
    void print_header(long, bool) const;
    static void print_general_info(const seed_results&);
    static void print_seed_info(sol_result);
    static void print_null_seed_info();
    static void print_seeds_in_prog(std::set<int>&);

    // Solving methods
    static sol_result solve_seed(int, millisec, const sol_rules&, uint64_t, bool);

    const sol_rules& rules;
    const uint64_t cache_capacity;
};


#endif //SOLVITAIRE_SOLVABILITY_H
