//
// Created by thecharlesblake on 10/24/17.
//

#include <string>
#include <ostream>
#include <iostream>
#include <algorithm>
#include <list>

#include "solver.h"
#include "../game/move.h"
#include "../input-output/output/log_helper.h"

using std::atomic;
using std::vector;
using std::cout;
using std::clog;
using std::pair;
using boost::optional;
using std::max;

solver::solver(const game_state& gs)
        : cache(lru_cache(gs, 1000000))
        , init_state(gs)
        , state(gs)
        , states_searched(1)
        , unique_states_searched(1)
        , backtracks(0)
        , dominance_moves(0)
        , depth(0)
        , max_depth(0)
        , root(nullptr, move(move::mtype::null))
        , current_node(&root) {
}

solver::~solver() {
    // This code looks to be unnecessary, but on alpha-star 6212 without it,
    // for reasons that I cannot fathom, it segfaults.
    while (current_node != nullptr) {
        current_node->children.clear();
        current_node = current_node->parent;
    }
}

solver::node::node(node* p, const move m, optional<lru_cache::item_list::iterator> c)
        : parent(p), mv(m), children(), parent_state_iter(c) {
}

solver::sol_state solver::run(boost::optional<atomic<bool> &> terminate_solver) {
    bool states_exhausted = false;

    while(!(state.is_solved() || states_exhausted)) {
        // If the terminate flag was supplied and has been set to true, return
        if (terminate_solver && *terminate_solver) {
            return sol_state::timed_out;
        }

#ifndef NDEBUG
        if (current_node->mv.type == move::mtype::dominance) {
            LOG_DEBUG("(dominance move)");
        }
        LOG_DEBUG(state);
#endif

        // If there is a dominance move available, adds it to the search tree
        // and repeats. Doesn't cache the state.
        optional<move> dominance_move = state.get_dominance_move();
        if (dominance_move) {
            // Adds the dominance move as a child of the current search node
            add_child(*dominance_move);
        } else {
            // Caches the current state
            pair<lru_cache::item_list::iterator, bool> insert_res = cache.insert(state);
            bool is_new_state = insert_res.second;
            if (is_new_state) {
                // Gets the legal moves in the current state
                vector<move> next_moves = get_next_moves();

                // If there are none, reverts to the last node with children
                if (next_moves.empty()) {
                    states_exhausted = revert_to_last_node_with_children(insert_res.first);
                } else {
                    add_children(next_moves, insert_res.first);
                }
            }
                // If the state is not a new one, reverts to the last node with children
            else {
                unique_states_searched--;
                states_exhausted = revert_to_last_node_with_children();
            }
        }

        // Sets the current node to one of its children
        assert(states_exhausted == current_node->children.empty());
        if (!states_exhausted) {
            current_node = &current_node->children.back();
            state.make_move(current_node->mv);
            depth++;
            max_depth = max(depth, max_depth);
            if (current_node->mv.type == move::mtype::dominance) dominance_moves++;
        }

        states_searched++;
        unique_states_searched++;
    }

    if (state.is_solved()) {
        return sol_state::solved;
    } else {
        assert(states_exhausted);
        return sol_state::unsolvable;
    }
}

void solver::add_children(std::vector<move>& moves, optional<lru_cache::item_list::iterator> i) {
    current_node->children.reserve(moves.size());
    for (auto move : moves) {
        add_child(move, i);
    }
}

void solver::add_child(move m, optional<lru_cache::item_list::iterator> i) {
    current_node->children.emplace_back(current_node, m, i);
}

vector<move> solver::get_next_moves() {
    return state.get_legal_moves(current_node->mv);
}

// Called when the current node's children have been exhausted. Travels back up
// the search tree until it finds a node which still has children. Returns true
// unless all children have been exhausted.

// If an iterator to the current state is supplied to the function, will also
// make sure to turn the 'live' bit off upon backtracking
bool solver::revert_to_last_node_with_children(optional<lru_cache::item_list::iterator> cur_state) {
    if (!current_node->parent)
        return true;

    // Turns the 'live' bit false on the state we are backtracking out of
    if (cur_state) cache.set_non_live(*cur_state);

    state.undo_move(current_node->mv);
    depth--;
    backtracks++;

#ifndef NDEBUG
    // Checks that the state after the undo is in the cache
    // (as long as the move wasn't a dominance move)

    if (current_node->mv.type != move::mtype::dominance) {
        if (cache.get_states_removed_from_cache() == 0) assert(cache.contains(state));
        LOG_DEBUG("(undo move)");
    } else {
        LOG_DEBUG("(undo dominance move)");
    }
    LOG_DEBUG(state);
#endif

    // Gets a reference to the parent state which can be supplied if this function is
    // called recursively. This ensures that the cached state's 'live' bit is set as appropriate
    optional<lru_cache::item_list::iterator> p_state = current_node->parent_state_iter;

    // Reverts the current node to its parent and removes it
    current_node = current_node->parent;
    current_node->children.pop_back();

    // If the current node now has no children, repeat
    if (current_node->children.empty()) {
        return revert_to_last_node_with_children(p_state);
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

    if (states_searched > 1) {
        while (!n->children.empty()) {
            state_copy.make_move(n->mv);
            cout << state_copy << "\n";
            n = &n->children.back();
        }
    }
    cout << "\n";
}

void solver::print_solution_info() const {
    cout << "States Searched: "           << states_searched                       << "\n"
         << "Unique States Searched: "    << unique_states_searched                << "\n"
         << "Backtracks: "                << backtracks                            << "\n"
         << "Dominance Moves: "           << dominance_moves                       << "\n"
         << "States Removed From Cache: " << cache.get_states_removed_from_cache() << "\n"
         << "Maximum Search Depth: "      << max_depth                             << "\n"
         << "Final Search Depth: "        << depth                                 << "\n";
}

const solver::node& solver::get_search_tree() const {
    return root;
}

int solver::get_states_searched() const {
    return states_searched;
}
