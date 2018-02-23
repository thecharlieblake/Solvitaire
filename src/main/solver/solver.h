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
        node(node*, game_state::move);
        node* parent;
        const game_state::move move;
        std::vector<node> children;
    };

    explicit solver(const game_state&);

    bool run();
    void print_solution() const;
    int get_states_searched() const;
    const node& get_search_tree() const;

private:
    bool revert_to_last_node_with_children();
    void add_children(std::vector<game_state::move>&);
    void add_child(game_state::move);

    const game_state init_state;
    game_state state;
    int states_searched;

    node root;
    node* current_node;
};


#endif //SOLVITAIRE_SOLVER_H
