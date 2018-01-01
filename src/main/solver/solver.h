//
// Created by thecharlesblake on 10/24/17.
//

#ifndef SOLVITAIRE_SOLVER_H
#define SOLVITAIRE_SOLVER_H

#include <string>
#include <vector>
#include <unordered_set>

#include "../game/game_state.h"
#include "../game/sol_rules.h"

class solver {
public:
    std::unordered_set<
            std::vector<pile>,
            boost::hash<std::vector<pile>>
    > global_cache;

    struct node {
        explicit node(game_state::move, std::vector<game_state::move>);
        const game_state::move move;
        std::vector<game_state::move> unsearched_children;
    };

    solver(const game_state&, const sol_rules&);

    bool run();
    void print_solution() const;

private:
    const game_state initial_state;
    const sol_rules rules;
    std::vector<node> frontier;
    int states_searched;
};


#endif //SOLVITAIRE_SOLVER_H
