//
// Created by thecharlesblake on 10/24/17.
//

#ifndef SOLVITAIRE_SOLVER_H
#define SOLVITAIRE_SOLVER_H

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include "game/game_state.h"
#include "game/sol_rules.h"

class solver {
public:
    solver(const game_state, const sol_rules&);
    bool run();

    friend std::ostream& operator<< (std::ostream&, const solver&);

private:
    struct node {
        node(const game_state);
        std::vector<game_state> history;
        game_state state;
    };

    const node root;
    const sol_rules rules;
    bool been_run;
    node solution;
    bool solution_found;
    int states_searched;
};


#endif //SOLVITAIRE_SOLVER_H
