//
// Created by thecharlesblake on 20/06/18.
//

#ifndef SOLVITAIRE_BINOMIAL_CI_H
#define SOLVITAIRE_BINOMIAL_CI_H


#include <utility>

struct binomial_ci {
    static std::pair<double, double> wilson(int, int);
    static std::pair<double, double> wilson(int, int, int);
    static std::pair<double, double> agresti_coull(int, int);
    static std::pair<double, double> agresti_coull(int, int, int);
};


#endif //SOLVITAIRE_BINOMIAL_CI_H
