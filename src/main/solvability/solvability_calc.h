//
// Created by thecharlesblake on 3/21/18.
//

#ifndef SOLVITAIRE_SOLVABILITY_H
#define SOLVITAIRE_SOLVABILITY_H

#include "../game/sol_rules.h"

class solvability_calc {
public:
    explicit solvability_calc(const sol_rules&);

    void calculate_solvability_percentage();

private:
    void print_header() const;
    void print_row(double, double) const;
    void solve_seed();
    void reset();

    double sol_lower_bound();
    double sol_upper_bound();
    static double agrestiCoull(int, int, double, bool);

    const sol_rules& rules;
    int current_seed;
    std::chrono::milliseconds timeout;
    int solvable;
    int unsolvable;
    int intractable;
};


#endif //SOLVITAIRE_SOLVABILITY_H
