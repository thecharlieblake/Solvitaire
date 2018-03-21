//
// Created by thecharlesblake on 3/21/18.
//

#ifndef SOLVITAIRE_SOLVABILITY_H
#define SOLVITAIRE_SOLVABILITY_H

#include "../game/sol_rules.h"

class solvability_calc {
public:
    explicit solvability_calc(const sol_rules&);

    void calculate_solvability_percentage() const;

private:
    static double sol_lower_bound(int, int, int);
    static double sol_upper_bound(int, int, int);
    static double agrestiCoull(int, int, double, bool);

    const sol_rules& rules;
};


#endif //SOLVITAIRE_SOLVABILITY_H
