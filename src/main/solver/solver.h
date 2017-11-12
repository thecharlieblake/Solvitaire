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
    struct node {
        node(const node*, const game_state&);
        std::vector<game_state> history;
        game_state state;

        static const std::vector<game_state> gen_history(const node* parent);
    };

    solver(const game_state, const sol_rules&);
    boost::optional<node> run();
    const node get_root() const;
    int get_states_searched() const;

    friend std::ostream& operator<< (std::ostream&, const solver::node&);

private:
    const node root;
    const sol_rules rules;
    bool been_run;
    node solution;
    bool solution_found;
    int states_searched;
};


#endif //SOLVITAIRE_SOLVER_H
