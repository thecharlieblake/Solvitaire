//
// Created by thecharlesblake on 10/24/17.
//

#include <string>
#include <ostream>
#include <iostream>
#include <algorithm>

#include "solver.h"
#include "log_helper.h"
#include "game_state.h"
#include <boost/log/trivial.hpp>
using namespace std;

solver::solver(const game_state gs, const sol_rules& sr)
        : root(gs), rules(sr), solution(gs), solution_found(false),
          states_searched(0) {}

solver::node::node(const game_state gs) : state(gs) {}

bool solver::run() {
    been_run = true;

    // Iteratively run a depth-first search. Nodes in the frontier are yet to
    // be explored
    vector<node> frontier;
    frontier.push_back(root);
    while (!frontier.empty()) {
        // Pop the element at the top of the stack
        node current = frontier.back();
        frontier.pop_back();

        LOG_DEBUG(current.state);

        // Create new nodes for each of the children
        vector<game_state> new_children = current.state.get_next_legal_states();
        states_searched++;

        // NOTE: get_next_legal_states() makes the is_solved() function work
        if (current.state.is_solved()) {
            solution = current;
            solution_found = true;
            break;
        }

        for (auto it = new_children.rbegin(); it != new_children.rend(); it++) {
            // If we have seen the state before, ignore it (loop detection))
            if (find(begin(current.history), end(current.history), *it)
                != end(current.history)) {
                continue;
            }

            node n = current;
            n.history.push_back(n.state);
            n.state = *it;

            frontier.push_back(n);
        }
    }

    return solution_found;
}

std::ostream& operator<< (std::ostream& stream, const solver& sol) {

    if (sol.solution_found) {
        stream << "Solution:\n";
        for (auto state : sol.solution.history) {
            stream << state << "\n";
        }
        stream << sol.solution.state << "\nSolved!\n";

    } else if (sol.been_run) {
        stream << "Deal:\n" << sol.root.state << "\n"
               << "No Possible Solution\n";
    } else {
        stream << "Solver has not been run yet\n";
    }
    stream << "States Searched: " << sol.states_searched << "\n";
    return stream;
}