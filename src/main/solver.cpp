//
// Created by thecharlesblake on 10/24/17.
//

#include <string>
#include <ostream>
#include <iostream>

#include "solver.h"
#include "log_helper.h"
#include "game_state.h"
#include <boost/log/trivial.hpp>
using namespace std;

solver::solver(const game_state gs) : root(gs), solution(gs),
                                      solution_found(false) {}

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

        LOG_DEBUG << current.state;

        // Create new nodes for each of the children
        vector<game_state> new_children = current.state.get_next_legal_states();
        for (auto it = new_children.rbegin(); it != new_children.rend(); it++) {
            node n = current;
            n.history.push_back(n.state);
            n.state = *it;

            frontier.push_back(n);
        }

        if (current.state.is_solved()) {
            solution = current;
            solution_found = true;
            break;
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
        stream << sol.solution.state << "\n";

    } else if (sol.been_run) {
        stream << "No Possible Solution\n";
    } else {
        stream << "Solver has not been run yet\n";
    }
    return stream;
}