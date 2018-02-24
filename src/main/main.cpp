#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <rapidjson/document.h>

#include <gperftools/profiler.h>

#include "input-output/input/command_line_helper.h"
#include "input-output/input/json-parsing/json_helper.h"
#include "input-output/input/json-parsing/rules_parser.h"
#include "input-output/output/log_helper.h"
#include "solver/solver.h"

using namespace rapidjson;

using namespace std;
using namespace boost;

namespace po = boost::program_options;

const optional<sol_rules> gen_rules(command_line_helper&);
void solve_random_game(int, const sol_rules&, bool);
void solve_input_files(vector<string>, const sol_rules&, bool);
void solve_game(const game_state&, bool);
void calculate_solvability_percentage(const sol_rules&);

int main(int argc, const char* argv[]) {

    // Parses the command-line options
    command_line_helper clh;
    if (!clh.parse(argc, argv)) {
        return EXIT_FAILURE;
    }

    // Generates the rules of the solitaire from the game type
    const optional<sol_rules> rules = gen_rules(clh);
    if (!rules) return EXIT_FAILURE;

    // If the user has asked for a solvability percentage, calculates it
    if (clh.get_solvability()) {
        calculate_solvability_percentage(*rules);
    }
    // If a random deal seed has been supplied, solves it
    else if (clh.get_random_deal() != -1) {
        solve_random_game(clh.get_random_deal(), *rules, clh.get_classify());
    }
    // Otherwise there are supplied input files which should be solved
    else {
        const vector<string> input_files = clh.get_input_files();

        // If there are no input files, solve a random deal based on the
        // supplied seed
        assert (!input_files.empty());
        solve_input_files(input_files, *rules, clh.get_classify());
    }

    return EXIT_SUCCESS;
}

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

void solve_random_game(int seed, const sol_rules& rules, bool classify) {
    LOG_INFO ("Attempting to solve with seed: " << seed << "...");
    game_state gs(rules, seed);
    solve_game(gs, classify);
}

void solve_input_files(const vector<string> input_files, const sol_rules& rules,
                       bool classify) {
    for (const string& input_file : input_files) {
        try {
            // Reads in the input file to a json doc
            const Document in_doc = json_helper::get_file_json(input_file);

            // Attempts to create a game state object from the json
            game_state gs(rules, in_doc);

            LOG_INFO ("Attempting to solve " << input_file << "...");
            solve_game(gs, classify);

        } catch (const runtime_error& error) {
            string errmsg = "Error parsing deal file: ";
            errmsg += error.what();
            LOG_ERROR(errmsg);
        }
    }
}

void solve_game(const game_state& gs, bool classify) {
    solver solv(gs);
    bool solution = solv.run();
    ProfilerStop();

    if (classify) {
        if (solution) {
            cout << "Solved\n";
        } else {
            cout << "No solution\n";
        }
    } else {
        if (solution) {
            solv.print_solution();
        } else {
            cout << "Deal:\n" << gs
                 << "\nNo Possible Solution\nStates Searched: "
                 << solv.get_states_searched() << "\n";
        }
    }
}

void calculate_solvability_percentage(const sol_rules& rules) {
    cout << "TODO: calculate solvability percentage";
    cout << rules.hole;
}
