#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <rapidjson/document.h>

#include "input-output/command_line_helper.h"
#include "input-output/deal_parser.h"
#include "game/game_state.h"
#include "solver/solver.h"
#include "input-output/log_helper.h"

using namespace rapidjson;

using namespace std;
using namespace boost;

namespace po = boost::program_options;

const optional<sol_rules> gen_rules(command_line_helper&);
void solve_random_game(int, const sol_rules&);
void solve_input_files(const vector<string>, const sol_rules);
void solve_game(const game_state&, const sol_rules&);

int main(int argc, const char* argv[]) {
    // Parses the command-line options
    command_line_helper clh;
    if (!clh.parse(argc, argv)) {
        return 1;
    }

    // Generates the rules of the solitaire from the game type
    const optional<sol_rules> rules = gen_rules(clh);
    if (!rules) return -1;

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

    return 0;
}

const optional<sol_rules> gen_rules(command_line_helper& clh) {
    if (!clh.get_solitaire_type().empty()) {
        return sol_rules::from_preset(clh.get_solitaire_type());
    } else {
        return sol_rules::from_file(clh.get_rules_file());
    }
}

void solve_random_game(int seed, const sol_rules& rules) {
    LOG_INFO ("Attempting to solve with seed: " << seed << "...");
    game_state gs(seed, rules);
    solve_game(gs, rules);
}

void solve_input_files(const vector<string> input_files, const sol_rules rules) {
    for (const auto &input_json : input_files) {
        // Attempts to read the user's json into a document
        Document doc;
        deal_parser::parse(doc, input_json);

        // Creates a game state object from the json, plus a solver
        game_state gs(doc);

        LOG_INFO ("Attempting to solve " << input_json << "...");
        solve_game(gs, rules);
    }
}

void solve_game(const game_state& gs, const sol_rules& rules) {
    solver sol(gs, rules);

    optional<solver::node> solution = sol.run();
    if (solution) {
        cout << "Solution:\n" << *solution << "States Searched: "
             << sol.get_states_searched() << "\n";
    } else {
        cout << "Deal:\n" << sol.get_root() << "\nNo Possible Solution\n";
    }
}
