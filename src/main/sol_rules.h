//
// Created by thecharlesblake on 10/28/17.
//

#ifndef SOLVITAIRE_SOL_RULES_H
#define SOLVITAIRE_SOL_RULES_H

#include <vector>
#include <string>

class sol_rules {
public:
    enum class valid_sol {
        BLACK_HOLE,
        SIMPLE_BLACK_HOLE,
        SPANISH_PATIENCE,
        SIMPLE_SPANISH_PATIENCE
    };
    enum class build_order {
        NO_BUILD,
        DESCENDING,
        ASCENDING,
        BOTH
    };
    enum class build_policy {
        CLUBS,
        DIAMONDS,
        HEARTS,
        SPADES,
        ANY_SUIT,
        SAME_SUIT
    };

    static std::vector<std::string> valid_sol_strs;
    static bool is_suit(build_policy);
    static int suit_val(build_policy);

    sol_rules(std::string);

    int tableau_pile_count;
    build_order build_ord;
    int max_rank;
    bool hole;
    bool foundations;

private:
    static valid_sol valid_sol_enum(const std::string&);
};

#endif //SOLVITAIRE_SOL_RULES_H
