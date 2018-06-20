#include <boost/program_options.hpp>
#include <boost/optional.hpp>

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

const optional<sol_rules> gen_rules(command_line_helper&);
void solve_random_game(int, const sol_rules&, bool, uint64_t);
void solve_input_files(vector<string>, const sol_rules&, bool, uint64_t);
void solve_game(const game_state&, bool, uint64_t);

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

    string game_rule_str = clh.get_describe_game_rules();
    if (!game_rule_str.empty()) {
        sol_preset_types::describe_game_rules(game_rule_str);
        return EXIT_SUCCESS;
    }

    // Generates the rules of the solitaire from the game type
    const optional<sol_rules> rules = gen_rules(clh);
    if (!rules) return EXIT_FAILURE;

    // If the user has asked for a solvability percentage, calculates it
    if (clh.get_solvability() > 0) {
        solvability_calc solv_c(*rules, clh.get_cache_capacity());
        solv_c.calculate_solvability_percentage(clh.get_solvability(), clh.get_cores(), clh.get_resume());
    }
        // If a random deal seed has been supplied, solves it
    else if (clh.get_random_deal() != -1) {
        solve_random_game(clh.get_random_deal(), *rules, clh.get_classify(), clh.get_cache_capacity());
    }
    // If the benchmark option has been supplied, generates it
    else if (clh.get_benchmark()) {
        benchmark::run(*rules, clh.get_cache_capacity());
    }
    // Otherwise there are supplied input files which should be solved
    else {
        const vector<string> input_files = clh.get_input_files();

        // If there are no input files, solve a random deal based on the
        // supplied seed
        assert (!input_files.empty());
        solve_input_files(input_files, *rules, clh.get_classify(), clh.get_cache_capacity());
    }

    return EXIT_SUCCESS;
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

void solve_random_game(int seed, const sol_rules& rules, bool classify, uint64_t cache_capacity) {
    LOG_INFO ("Attempting to solve with seed: " << seed << "...");
    game_state gs(rules, seed);
    solve_game(gs, classify, cache_capacity);
}

void solve_input_files(const vector<string> input_files, const sol_rules& rules,
                       bool classify, uint64_t cache_capacity) {
    for (const string& input_file : input_files) {
        try {
            // Reads in the input file to a json doc
            const Document in_doc = json_helper::get_file_json(input_file);

            // Attempts to create a game state object from the json
            game_state gs(rules, in_doc);

            LOG_INFO ("Attempting to solve " << input_file << "...");
            solve_game(gs, classify, cache_capacity);

        } catch (const runtime_error& error) {
            string errmsg = "Error parsing deal file: ";
            errmsg += error.what();
            LOG_ERROR(errmsg);
        }
    }
}

void solve_game(const game_state& gs, bool classify, uint64_t cache_capacity) {
    solver solv(gs, cache_capacity);

    bool solution = solv.run() == solver::sol_state::solved;

    if (solution) {
        if (!classify) solv.print_solution();
        cout << "Solved\n";
    } else {
        if (!classify) cout << "Deal:\n" << gs << "\n";
        cout << "No Possible Solution\n";
    }

    cout << solv.get_solution_info();
}
