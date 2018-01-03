#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <rapidjson/document.h>

#include "input-output/command_line_helper.h"
#include "input-output/log_helper.h"
#include "input-output/json_helper.h"
#include "input-output/rules_parser.h"
#include "solver/solver.h"

using namespace rapidjson;

using namespace std;
using namespace boost;

namespace po = boost::program_options;

const optional<sol_rules> gen_rules(command_line_helper&);
void solve_random_game(int, const sol_rules&);
void solve_input_files(vector<string>, const sol_rules&);
void solve_game(const game_state&, const sol_rules&);

int main(int argc, const char* argv[]) {

    // Parses the command-line options
    command_line_helper clh;
    if (!clh.parse(argc, argv)) {
        return EXIT_FAILURE;
    }

    // Generates the rules of the solitaire from the game type
    const optional<sol_rules> rules = gen_rules(clh);
    if (!rules) return EXIT_FAILURE;

    // Retrieves the input files to be solved
    const vector<string> input_files = clh.get_input_files();

    // If there are no input files, solve a random deal based on the
    // supplied seed
    if (input_files.empty()) {
        solve_random_game(clh.get_random_deal(), *rules);
    }
    // Otherwise, solve the input files
    else {
        solve_input_files(input_files, *rules);
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

void solve_random_game(int seed, const sol_rules& rules) {
    LOG_INFO ("Attempting to solve with seed: " << seed << "...");
    game_state gs(rules, seed);
    solve_game(gs, rules);
}

void solve_input_files(const vector<string> input_files, const sol_rules& rules) {
    for (const string& input_file : input_files) {
        try {
            // Reads in the input file to a json doc
            const Document in_doc = json_helper::get_file_json(input_file);

            // Attempts to create a game state object from the json
            game_state gs(rules, in_doc);

            LOG_INFO ("Attempting to solve " << input_file << "...");
            solve_game(gs, rules);

        } catch (const runtime_error& error) {
            string errmsg = "Error parsing deal file: ";
            errmsg += error.what();
            LOG_ERROR(errmsg);
        }
    }
}

void solve_game(const game_state& gs, const sol_rules& rules) {
    solver solv(gs, rules);
    bool solution = solv.run();

    if (solution) {
        solv.print_solution();
    } else {
        cout << "Deal:\n" << gs << "\nNo Possible Solution\n";
    }
}
