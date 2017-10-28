//
// Created by thecharlesblake on 10/28/17.
//

#ifndef SOLVITAIRE_SOL_RULES_H
#define SOLVITAIRE_SOL_RULES_H

#include <vector>
#include <string>

struct sol_rules {
    static enum class valid_sol;
    static std::vector<std::string> valid_sol_strs;

    sol_rules(std::string);

    int tableau_pile_count;
    int max_rank;
    bool hole;
    bool foundations;

private:
    static valid_sol valid_sol_enum(const std::string&);
};


#endif //SOLVITAIRE_SOL_RULES_H
