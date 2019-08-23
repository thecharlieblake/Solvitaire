/*
  Solvitaire: a solver for perfect information solitaire games
  Copyright (C) 2018 Charles Blake <thecharlesblake@live.co.uk> and 
  Ian Gent <Ian.Gent@st-andrews.ac.uk>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program (see LICENSE file); if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include <boost/program_options.hpp>
#include <boost/optional.hpp>

#include "version.h"
#include "../../lib/rapidjson/document.h"
#include "input-output/input/command_line_helper.h"
#include "input-output/input/sol_preset_types.h"
#include "input-output/input/json-parsing/json_helper.h"
#include "input-output/input/json-parsing/rules_parser.h"
#include "input-output/output/log_helper.h"
#include "solver/solver.h"
#include "evaluation/solvability_calc.h"
#include "evaluation/benchmark.h"

using namespace rapidjson;

using namespace std;
using namespace boost;

namespace po = boost::program_options;

typedef std::chrono::milliseconds millisec;

const optional<sol_rules> gen_rules(command_line_helper&);
void solve_random_game(int, const sol_rules&, command_line_helper&);
void solve_input_files(vector<string>, const sol_rules&, command_line_helper&);
void solve_game(const sol_rules& rules, command_line_helper& clh, optional<int> seed, optional<const Document&> in_doc);
pair<solver, solver::result> solve_game(const sol_rules& rules, uint64_t timeout, uint64_t cache_capacity,
                                        game_state::streamliner_options str_opts,
                                        optional<int> seed, optional<const Document&> in_doc);
void print_version();

// Decides what to do given supplied command-line options
int main(int argc, const char* argv[]) {

    // Parses the command-line options
    command_line_helper clh;
    if (!clh.parse(argc, argv)) {
        return EXIT_FAILURE;
    }

    // If the user has asked for the list of preset game types, prints it
    if (clh.get_available_game_types()) {
        sol_preset_types::print_available_games();
        return EXIT_SUCCESS;
    }

    // If the user has asked for the version, prints it
    if (clh.get_version()) {
        print_version();
        return EXIT_SUCCESS;
    }

    string game_rule_str = clh.get_describe_game_rules();
    if (!game_rule_str.empty()) {
        sol_preset_types::describe_game_rules(game_rule_str);
        return EXIT_SUCCESS;
    }

    // Generates the rules of the solitaire from the game type
    const optional<sol_rules> rules = gen_rules(clh);
    if (!rules) return EXIT_FAILURE;

    if (clh.get_deal_only()) {
        game_state gs(*rules, clh.get_random_deal(), game_state::streamliner_options::NONE);
        json_helper::print_game_state_as_json(gs);
        return EXIT_SUCCESS;
    }

    // If the user has asked for a solvability percentage, calculates it
    if (clh.get_solvability() > 0) {
        solvability_calc solv_c(*rules, clh.get_cache_capacity());
        solv_c.calculate_solvability_percentage(clh.get_timeout(), clh.get_solvability(), clh.get_cores(),
                                                clh.get_streamliners(), clh.get_resume());
    }
        // If a random deal seed has been supplied, solves it
    else if (clh.get_random_deal() != -1) {
        solve_random_game(clh.get_random_deal(), *rules, clh);
    }
    // If the benchmark option has been supplied, generates it
    else if (clh.get_benchmark()) {
        benchmark::run(*rules, clh.get_cache_capacity(), clh.get_streamliners_game_state());
    }
    // Otherwise there are supplied input files which should be solved
    else {
        const vector<string> input_files = clh.get_input_files();

        // If there are no input files, solve a random deal based on the
        // supplied seed
        assert(!input_files.empty());
        solve_input_files(input_files, *rules, clh);
    }

    return EXIT_SUCCESS;
}

void print_version() {
    LOG_INFO("(Solvitaire version: " << SOLVITAIRE_VERSION_H << ")");
}

// Generates the game rules given the command line options
const optional<sol_rules> gen_rules(command_line_helper& clh) {
    try {
        if (!clh.get_solitaire_type().empty()) {
            return rules_parser::from_preset(clh.get_solitaire_type());
        } else {
            return rules_parser::from_file(clh.get_rules_file());
        }
    } catch (const runtime_error& error) {
        string errmsg = "Error in rules generation: ";
        errmsg += error.what();
        LOG_ERROR(errmsg);
        return none;
    }
}

void solve_random_game(int seed, const sol_rules& rules, command_line_helper& clh) {
    if (!clh.get_classify())
        LOG_INFO ("Attempting to solve with seed: " << seed << "...");
    solve_game(rules, clh, seed, none);
}

void solve_input_files(const vector<string> input_files, const sol_rules& rules, command_line_helper& clh) {
    for (const string& input_file : input_files) {
        try {
            // Reads in the input file to a json doc
            const Document in_doc = json_helper::get_file_json(input_file);

            LOG_INFO ("Attempting to solve " << input_file << "...");
            solve_game(rules, clh, none, in_doc);

        } catch (const runtime_error& error) {
            string errmsg = "Error parsing deal file: ";
            errmsg += error.what();
            LOG_ERROR(errmsg);
        }
    }
}

void solve_game(const sol_rules& rules, command_line_helper& clh, optional<int> seed, optional<const Document&> in_doc) {
    typedef pair<solver, solver::result> solve_sol;

    bool smart = clh.get_streamliners() == command_line_helper::streamliner_opt::SMART;

    uint64_t timeout;
    game_state::streamliner_options str_opt;
    if (smart) {
        timeout = clh.get_timeout() / 10;
        str_opt = game_state::streamliner_options::BOTH;
    } else {
        timeout = clh.get_timeout();
        str_opt = clh.get_streamliners_game_state();
    }
    solve_sol solution = solve_game(rules, timeout, clh.get_cache_capacity(), str_opt, seed, in_doc);

    bool run_again = smart && solution.second.sol_type != solver::result::type::SOLVED;
    cout.flush();
    if (run_again)
        if (!clh.get_classify()) cout << "Unsolvable using streamliner. Running again...\n";
    optional<solve_sol> streamliner_solution = run_again
            ? solve_game(rules, clh.get_timeout(), clh.get_cache_capacity(), game_state::streamliner_options::NONE, seed, in_doc)
            : optional<solve_sol>();

    if (clh.get_classify()) {
        if (seed) cout << *seed;
        solver::print_result_csv(solution.second);
        if (smart) {
            if (run_again) {
                solver::print_result_csv(streamliner_solution->second);
                cout << ", " << streamliner_solution->second.sol_type;
            } else {
                solver::print_null_seed_info();
                cout << ", " << solution.second.sol_type;
            }
        } else {
            cout << ", " << solution.second.sol_type;
        }
        cout << "\n";
    } else {
        pair<solver, solver::result> s = run_again ? *streamliner_solution : solution;

        if (s.second.sol_type == solver::result::type::SOLVED) {
            s.first.print_solution();
        } else {
            cout << "Deal:\n" << s.first.init_state << "\n";
        }
        cout << s.second;
    }
    cout.flush();
}

pair<solver, solver::result> solve_game(const sol_rules& rules, uint64_t timeout, uint64_t cache_capacity,
                                        game_state::streamliner_options str_opts,
                                        optional<int> seed, optional<const Document&> in_doc) {
    game_state gs = seed ? game_state(rules, *seed, str_opts) : game_state(rules, *in_doc, str_opts);
    solver sol(gs, cache_capacity);
    solver::result res = sol.run(std::chrono::milliseconds(timeout));
    return make_pair(sol, res);
}
