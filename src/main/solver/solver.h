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
        node(node*, move, boost::optional<lru_cache::item_list::iterator> = boost::none);
        node* parent;
        const move mv;
        std::vector<node> children;
        boost::optional<lru_cache::item_list::iterator> parent_state_iter; // Optional, as dominance moves aren't cached
    };

    explicit solver(const game_state&, uint64_t);
    ~solver();

    sol_state run(boost::optional<std::atomic<bool> &> = boost::none);

    void print_solution() const;
    void print_solution_info() const;
    int get_states_searched() const;
    int get_unique_states_searched() const;
    int get_cache_size() const;
    const node& get_search_tree() const;

private:
    bool revert_to_last_node_with_children(boost::optional<lru_cache::item_list::iterator> = boost::none);
    void add_children(std::vector<move>&, boost::optional<lru_cache::item_list::iterator> = boost::none);
    void add_child(move, boost::optional<lru_cache::item_list::iterator> = boost::none);

    const game_state init_state;
    game_state state;

    int states_searched;
    int unique_states_searched;
    int backtracks;
    int dominance_moves;
    int depth;
    int max_depth;

    node root;
    node* current_node;
};


#endif //SOLVITAIRE_SOLVER_H
