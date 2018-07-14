//
// Created by thecharlesblake on 10/24/17.
//

#include <string>
#include <ostream>
#include <iostream>
#include <algorithm>
#include <list>
#include <malloc.h>

#include "solver.h"
#include "../game/move.h"
#include "../input-output/output/log_helper.h"
#include "../input-output/output/state_printer.h"

using std::atomic;
using std::vector;
using std::cout;
using std::clog;
using std::pair;
using std::max;
using std::begin;
using std::end;
using boost::optional;

solver::solver(const game_state& gs, uint64_t cache_capacity)
        : cache(lru_cache(gs, cache_capacity))
        , init_state(gs)
        , state(gs)
        , frontier()
        , states_searched(0)
        , unique_states_searched(0)
        , backtracks(0)
        , dominance_moves(0)
        , depth(0)
        , max_depth(0)
        , root(move(move::mtype::null))
        , current_node() {
    frontier.push_back(root);
    current_node = begin(frontier);
}

solver::node::node(const move m)
        : mv(m), child_moves(), cache_state() {
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
            // Adds the dominance move as a child of the current search node;
            current_node->child_moves.emplace_back(*dominance_move);
        } else {
            // Caches the current state
            pair<lru_cache::item_list::iterator, bool> insert_res = cache.insert(state);
            current_node->cache_state = insert_res.first;
            bool is_new_state = insert_res.second;

            if (is_new_state) {
                // Gets the legal moves in the current state
                vector<move> next_moves = state.get_legal_moves(current_node->mv);

                // If there are none, reverts to the last node with children
                if (next_moves.empty()) {
                    states_exhausted = revert_to_last_node_with_children(insert_res.first);
                } else {
                    current_node->child_moves = std::move(next_moves);
                }
            }
                // If the state is not a new one, reverts to the last node with children
            else {
                unique_states_searched--;
                states_exhausted = revert_to_last_node_with_children();
            }
        }

        // Sets the current node to one of its children
        assert(states_exhausted == current_node->child_moves.empty());
        if (!states_exhausted) {
            set_to_child();
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

// Called when the current node's children have been exhausted. Travels back up
// the search tree until it finds a node which still has children. Returns true
// unless all children have been exhausted.

// If an iterator to the current state is supplied to the function, will also
// make sure to turn the 'live' bit off upon backtracking
bool solver::revert_to_last_node_with_children(optional<lru_cache::item_list::iterator> cur_state) {
    if (current_node == begin(frontier))
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
        if (cache.get_states_removed_from_cache() == 0) {
            assert(cache.contains(state));
        }
        LOG_DEBUG("(undo move)");
    } else {
        LOG_DEBUG("(undo dominance move)");
    }
    LOG_DEBUG(state);
#endif

    // Gets a reference to the parent state which can be supplied if this function is
    // called recursively. This ensures that the cached state's 'live' bit is set as appropriate
    optional<lru_cache::item_list::iterator> p_state = prev(current_node)->cache_state;

    // Reverts the current node to its parent and removes it
    frontier.pop_back();
    current_node = prev(end(frontier));

    // If the current node now has no children, repeat
    if (current_node->child_moves.empty()) {
        return revert_to_last_node_with_children(p_state);
    } else {
        return false;
    }
}

void solver::set_to_child() {
    assert(!current_node->child_moves.empty());

    move b = current_node->child_moves.back();
    current_node->child_moves.pop_back();
    frontier.emplace_back(b);

    current_node = prev(end(frontier));
}

void solver::print_solution() const {
    std::flush(clog);
    std::flush(cout);

    auto i = begin(frontier);
    game_state state_copy = init_state;

    cout << "Solution:\n";
    cout << state_copy << "\n";

    if (states_searched > 1) {
        while (++i != end(frontier)) {
            state_copy.make_move(i->mv);
            cout << state_copy << "\n";
        }
    }
    cout << "\n";
}

std::ostream& operator<< (std::ostream& out, const solver::solution_info& si) {
    return out
            << "States Searched: "           << si.states_searched            << "\n"
            << "Unique States Searched: "    << si.unique_states_searched     << "\n"
            << "Backtracks: "                << si.backtracks                 << "\n"
            << "Dominance Moves: "           << si.dominance_moves            << "\n"
            << "States Removed From Cache: " << si.states_removed_from_cache  << "\n"
            << "Final States In Cache: "     << si.cache_size                 << "\n"
            << "Final Buckets In Cache: "    << si.cache_bucket_count         << "\n"
            << "Maximum Search Depth: "      << si.max_depth                  << "\n"
            << "Final Search Depth: "        << si.depth                      << "\n";
}

solver::solution_info solver::get_solution_info() {
    solution_info si;
    si.states_searched = states_searched;
    si.unique_states_searched = unique_states_searched;
    si.backtracks = backtracks;
    si.dominance_moves = dominance_moves;
    si.states_removed_from_cache = cache.get_states_removed_from_cache();
    si.cache_size = cache.size();
    si.cache_bucket_count = cache.bucket_count();
    si.max_depth = max_depth;
    si.depth = depth;
    return si;
}

const vector<solver::node> solver::get_frontier() const {
    return frontier;
}

int solver::get_states_searched() const {
    return states_searched;
}

int solver::get_unique_states_searched() const {
    return unique_states_searched;
}

int solver::get_cache_size() const {
    return cache.size();
}

int solver::get_states_rem_from_cache() const {
    return cache.get_states_removed_from_cache();
}
