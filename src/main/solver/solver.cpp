//
// Created by thecharlesblake on 10/24/17.
//

#include <string>
#include <ostream>
#include <iostream>
#include <algorithm>

#include "solver.h"
#include "../input-output/output/log_helper.h"

using namespace std;
using namespace boost;

solver::solver(const game_state& gs, const sol_rules& sr)
        : initial_state(gs), rules(sr), states_searched(0) {}

solver::node::node(const game_state::move m,
                   vector<game_state::move> uc)
        : move(m), unsearched_children(std::move(uc)) {
}

bool solver::run() {
    game_state state = initial_state;

    states_searched++;
    LOG_DEBUG (state);
    frontier.emplace_back(game_state::null_move(), state.get_legal_moves());

    while (!frontier.empty()) {
        node& current = frontier.back();

        // If we have a solution, returns true
        if (state.is_solved()) return true;

        // If the current node has no unsearched children
        if (current.unsearched_children.empty()) {
            // Undoes the move that led to this state
            // (unless it's the null first move)
            if (frontier.size() > 1) {
                state.undo_move(current.move);
            }
            // Returns to the previous state
            frontier.pop_back();
        } else {
            game_state::move next_move = current.unsearched_children[current.unsearched_children.size()-1];
            current.unsearched_children.pop_back();

            // Applies the first possible move in this state
            state.make_move(next_move);

            // Insert the state into the global cache
            bool is_new_state = global_cache.insert(state.get_data()).second;
            if (is_new_state) {
                states_searched++;
                LOG_DEBUG (state);
                // If the state is new, creates a new node in the frontier to
                // represent the move just made, and those that are now possible
                frontier.emplace_back(next_move, state.get_legal_moves());
            } else {
                // If we've seen the state before, undoes the move
                state.undo_move(next_move);
            }
        }
    }

    return false;
}

void solver::print_solution() const {
    std::flush(clog);
    std::flush(cout);
    game_state state = initial_state;

    cout << "Solution:\n";
    cout << state << "\n";
    // Ignores first move which is
    bool first = true;
    for (auto f_node : frontier) {
        if (first) {
            first = false;
            continue;
        }
        // TODO print move as well
        state.make_move(f_node.move);
        cout << state << "\n";
    }
    cout << "States Searched: " << states_searched << "\n";
}
