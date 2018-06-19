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
    enum class sol_state {solved, unsolvable, timed_out};
    lru_cache cache;

    struct node {
        node(move);
        const move mv;
        std::vector<move> child_moves;
        boost::optional<lru_cache::item_list::iterator> cache_state; // Optional, as dominance moves aren't cached
    };

    explicit solver(const game_state&, uint64_t);

    sol_state run(boost::optional<std::atomic<bool> &> = boost::none);

    void print_solution() const;
    void print_solution_info() const;
    int get_states_searched() const;
    int get_unique_states_searched() const;
    int get_cache_size() const;
    int get_states_rem_from_cache() const;
    const std::vector<node> get_frontier() const;

private:
    bool revert_to_last_node_with_children(boost::optional<lru_cache::item_list::iterator> = boost::none);
    void set_to_child();

    const game_state init_state;
    game_state state;
    std::vector<node> frontier;

    int states_searched;
    int unique_states_searched;
    int backtracks;
    int dominance_moves;
    int depth;
    int max_depth;

    node root;
    std::vector<node>::iterator current_node;
};


#endif //SOLVITAIRE_SOLVER_H
