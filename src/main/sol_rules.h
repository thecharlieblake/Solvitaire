//
// Created by thecharlesblake on 10/28/17.
//

#ifndef SOLVITAIRE_SOL_RULES_H
#define SOLVITAIRE_SOL_RULES_H

#include <vector>
#include <string>

struct sol_rules {
    enum class valid_sol {
        BLACK_HOLE,
        SIMPLE_BLACK_HOLE,
        SPANISH_PATIENCE
    };
    enum class tab_build_order {
        NO_BUILD,
        DESCENDING,
    };

    static std::vector<std::string> valid_sol_strs;

    sol_rules(std::string);

    int tableau_pile_count;
    tab_build_order build_order;
    int max_rank;
    bool hole;
    bool foundations;

private:
    static valid_sol valid_sol_enum(const std::string&);
};


#endif //SOLVITAIRE_SOL_RULES_H
