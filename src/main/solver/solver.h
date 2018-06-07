//
// Created by thecharlesblake on 10/24/17.
//

#ifndef SOLVITAIRE_SOLVER_H
#define SOLVITAIRE_SOLVER_H

#include <string>
#include <vector>
#include <atomic>

#include "../game/global_cache.h"
#include "../game/sol_rules.h"

class solver {
public:
    enum class sol_state {solved, unsolvable, cutoff};
    global_cache cache;

    struct node {
        node(node*, game_state::move);
        node* parent;
        const game_state::move move;
        std::vector<node> children;
    };

    explicit solver(const game_state&);
    ~solver();

    sol_state run(boost::optional<std::atomic<bool> &> = boost::none);

    void print_solution() const;
    int get_states_searched() const;
    int get_final_depth() const;
    const node& get_search_tree() const;
    int get_backtrack_count();

private:
    bool revert_to_last_node_with_children();
    void add_children(std::vector<game_state::move>&);
    void add_child(game_state::move);
    std::vector<game_state::move> get_next_moves();

    const game_state init_state;
    game_state state;
    int states_searched;
    int backtracks;
    uint depth;

    node root;
    node* current_node;
};


#endif //SOLVITAIRE_SOLVER_H
