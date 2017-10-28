#include <iostream>

#include <boost/program_options.hpp>
#include <rapidjson/document.h>

#include "command_line_helper.h"
#include "deal_parser.h"
#include "game_state.h"
#include "solver.h"
#include "sol_rules.h"

using namespace rapidjson;

using namespace std;

namespace po = boost::program_options;

void solve_game(const game_state&, const sol_rules&);

int main(int argc, const char* argv[]) {
    // Parses the command-line options
    command_line_helper clh;
    if (!clh.parse(argc, argv)) {
        return 1;
    }

    // Generates the rules of the solitaire from the game type
    const sol_rules rules(clh.get_solitaire_type());

    int seed = clh.get_random_deal();
    if (seed >= 0) {
        cout << "Attempting to solve with seed: " << seed << "...\n";
        game_state gs(seed, sol_rules);
        solve_game(gs, sol_rules);
    }

    for (auto input_json : clh.get_input_files()) {
        try {
            // Attempts to read the user's json into a document
            Document doc;
            deal_parser::parse(doc, input_json);

            // Creates a game state object from the json, plus a solver
            game_state gs(doc, clh.get_solitaire_type());

            cout << "Attempting to solve " << input_json << "...\n";
            solve_game(gs, sol_rules);

        } catch (runtime_error& e) {
            cerr << "Error: " << e.what() << "\n";
        }
    }

    return 0;
}

void solve_game(const game_state& gs, const sol_rules& rules) {
    solver sol(gs, rules);

    sol.run();
    cout << sol;
}
