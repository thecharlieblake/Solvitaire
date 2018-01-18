//
// Created by thecharlesblake on 10/24/17.
//

#ifndef SOLVITAIRE_SOLVER_H
#define SOLVITAIRE_SOLVER_H

#include <string>
#include <vector>

#include "../game/global_cache.h"
#include "../game/sol_rules.h"

class solver {
public:
    global_cache cache;

    struct node {
        explicit node(game_state::move, std::vector<game_state::move>);
        const game_state::move move;
        std::vector<game_state::move> unsearched_children;
    };

    explicit solver(const game_state&);

    bool run();
    void print_solution() const;

private:
    const game_state initial_state;
    std::vector<node> frontier;
    int states_searched;
};


#endif //SOLVITAIRE_SOLVER_H
