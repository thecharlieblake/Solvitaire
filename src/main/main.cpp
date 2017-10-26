#include <iostream>

#include <boost/program_options.hpp>
#include <rapidjson/document.h>

#include "command_line_helper.h"
#include "card.h"
#include "deal_parser.h"
#include "game_state.h"
#include "solver.h"

using namespace rapidjson;

using namespace std;

namespace po = boost::program_options;

int main(int argc, const char* argv[]) {
    // Parses the command-line options
    command_line_helper clh;
    if (!clh.parse(argc, argv)) {
        return 1;
    }

    for (auto input_json : clh.get_input_files()) {
        try {
            // Attempts to read the user's json into a document
            Document doc;
            deal_parser::parse(doc, input_json);

            // Creates a game state object from the json, plus a solver
            game_state gs(doc);
            solver sol(gs);

            cout << "Attempting to solve " << input_json << "...\n";
            sol.run();
            cout << sol;

        } catch (runtime_error& e) {
            cerr << "Error: " << e.what() << "\n";
        }
    }

    return 0;
}
