//
// Created by thecharlesblake on 10/24/17.
//

#include <string>
#include <ostream>
#include <iostream>
#include <algorithm>

#include "solver.h"
#include "input-output/log_helper.h"
#include "game/game_state.h"
#include <boost/log/trivial.hpp>

using namespace std;
using namespace boost;

solver::solver(const game_state gs, const sol_rules& sr)
        : root(NULL, gs), rules(sr), solution(root), states_searched(0) {}

solver::node::node(const node* parent, const game_state& gs) :
        history(solver::node::gen_history(parent)), state(gs) {}

const vector<game_state> solver::node::gen_history(const node* parent) {
    vector<game_state> hist;
    if (parent != NULL) {
        hist = std::vector<game_state>(parent->history);
        hist.push_back(parent->state);
    } else {
        hist = vector<game_state>();
    }
    return hist;
}

const optional<solver::node> solver::run() {
    // Iteratively run a depth-first search. Nodes in the frontier are yet to
    // be explored
    vector<node> frontier;
    frontier.push_back(root);

    while (!frontier.empty()) {
        // Pop the element at the top of the stack
        const node current = frontier.back();
        frontier.pop_back();

        LOG_DEBUG (current.state);

        // Create new nodes for each of the children
        vector<game_state> new_children = current.state.get_next_legal_states();
        states_searched++;

        for (auto it = new_children.rbegin(); it != new_children.rend(); it++) {
            // If we have seen the state before, ignore it (loop detection))
            if (find(begin(current.history), end(current.history), *it)
                != end(current.history)) {
                continue;
            }

            const node n(&current, *it);
            if (n.state.is_solved()) {
                return n;
            } else {
                frontier.push_back(n);
            }
        }
    }

    return none;
}

const solver::node solver::get_root() const {
    return root;
}

int solver::get_states_searched() const {
    return states_searched;
}

std::ostream& operator<< (std::ostream& stream, const solver::node& solution) {
    for (auto state : solution.history) {
        stream << state << "\n";
    }
    stream << solution.state << "\n";
    return stream;
}