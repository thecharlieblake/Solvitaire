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
    lru_cache cache;

    struct node {
        node(move);
        const move mv;
        std::vector<move> child_moves;
        boost::optional<lru_cache::item_list::iterator> cache_state; // Optional, as dominance moves aren't cached
    };

    struct result {
        enum class type { TIMEOUT, SOLVED, UNSOLVABLE, MEM_LIMIT };

        type sol_type;
        int states_searched;
        int unique_states_searched;
        int backtracks;
        int dominance_moves;
        uint64_t states_removed_from_cache;
        lru_cache::item_list::size_type cache_size;
        lru_cache::item_list::size_type cache_bucket_count;
        int max_depth;
        int depth;
        std::chrono::milliseconds time;
    };

    explicit solver(const game_state&, uint64_t);

    result run(boost::optional<std::chrono::milliseconds> = boost::none);

    void print_solution() const;
    const std::vector<node> get_frontier() const;

private:
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::milliseconds millisec;

    result::type dfs(boost::optional<clock::time_point> = boost::none);

    bool revert_to_last_node_with_children(boost::optional<lru_cache::item_list::iterator> = boost::none);
    void set_to_child();

    const game_state init_state;
    game_state state;
    std::vector<node> frontier;

    result res;

    node root;
    std::vector<node>::iterator current_node;
};

std::ostream& operator<< (std::ostream&, const solver::result::type&);
std::ostream& operator<< (std::ostream&, const solver::result&);


#endif //SOLVITAIRE_SOLVER_H
