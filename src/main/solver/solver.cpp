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

solver::solver(const game_state& gs)
        : cache(gs)
        , init_state(gs)
        , state(gs)
        , states_searched(1)
        , root(nullptr, game_state::move(0, 0))
        , current_node(&root) {
}

solver::node::node(node* p, const game_state::move m)
        : parent(p), move(m) {
}

bool solver::run() {
    bool states_exhausted = false;
    while(!state.is_solved() && !states_exhausted) {
#ifndef NDEBUG
        if (current_node->move.is_dominance())
            LOG_DEBUG("(dominance move)");
        LOG_DEBUG(state);
#endif

        // If there is a dominance move available, adds it to the search tree
        // and repeats. Doesn't cache the state.
        optional<game_state::move> dominance_move = state.get_dominance_move();
        if (dominance_move) {
            // Adds the dominance move as a child of the current search node
            add_child(*dominance_move);
        } else {
            // Caches the current state
            bool is_new_state = cache.insert(state);
            if (is_new_state) {
                // Gets the legal moves in the current state
                vector<game_state::move> next_moves = state.get_legal_moves();

                // If there are none, reverts to the last node with children
                if (state.get_legal_moves().empty()) {
                    states_exhausted = revert_to_last_node_with_children();
                } else {
                    add_children(next_moves);
                }
            }
            // If the state is not a new one, reverts to the last node with children
            else {
                states_exhausted = revert_to_last_node_with_children();
            }
        }

        // Sets the current node to one of its children
        assert(states_exhausted == current_node->children.empty());
        if (!states_exhausted) {
            current_node = &current_node->children.back();
            state.make_move(current_node->move);
        }

        states_searched++;
    }

    return state.is_solved();
}

void solver::add_children(std::vector<game_state::move>& moves) {
    for (auto move : moves) {
        add_child(move);
    }
}

void solver::add_child(game_state::move move) {
    current_node->children.emplace_back(current_node, move);
}

// Called when the current node's children have been exhausted. Travels back up
// the search tree until it finds a node which still has children. Returns true
// unless all children have been exhausted.
bool solver::revert_to_last_node_with_children() {
    if (!current_node->parent)
        return true;

    state.undo_move(current_node->move);

#ifndef NDEBUG
    // Checks that the state after the undo is in the cache
    // (as long as the move wasn't a dominance move)

    if (!current_node->move.is_dominance()) {
        assert(cache.contains(state));
        LOG_DEBUG("(undo move)");
    } else {
        LOG_DEBUG("(undo dominance move)");
    }
    LOG_DEBUG(state);
#endif

    // Reverts the current node to its parent and removes it
    current_node = current_node->parent;
    current_node->children.pop_back();

    // If the current node now has no children, repeat
    if (current_node->children.empty()) {
        return revert_to_last_node_with_children();
    } else {
        return false;
    }
}

void solver::print_solution() const {
    std::flush(clog);
    std::flush(cout);

    const node* n = &root.children.back();
    game_state state_copy = init_state;

    cout << "Solution:\n";
    cout << state_copy << "\n";
    // Ignores first move which is
    while (!n->children.empty()) {
        // TODO print move as well
        state_copy.make_move(n->move);
        cout << state_copy << "\n";
        n = &n->children.back();
    }
    cout << "States Searched: " << states_searched << "\n";
}

int solver::get_states_searched() const {
    return states_searched;
}

const solver::node& solver::get_search_tree() const {
    return root;
}
