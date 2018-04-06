//
// Created by thecharlesblake on 10/16/17.
//

#include <tuple>
#include <vector>

#include <boost/program_options.hpp>

#include "command_line_helper.h"
#include "../../game/sol_rules.h"
#include "../output/log_helper.h"
#include "sol_preset_types.h"

using namespace std;

namespace po = boost::program_options;

command_line_helper::command_line_helper()
        : main_options("options") {

    main_options.add_options()
            ("help", "produce help message")
            ("type", po::value<string>(),
             "specify the type of the solitaire game to be solved from the list "
                     "of preset games. Must supply either this 'type' option, "
                     "or the 'custom-rules' option")
            ("available-game-types", "outputs a list of the different preset "
                    "game types that can be solved")
            ("describe-game-rules", po::value<string>(),
                    "outputs the JSON that describes the rules of the supplied "
                    "preset game type")
            ("custom-rules", po::value<string>(),
             "the path to a JSON file describing the rules of the solitaire "
                     "to be solved. Must supply either 'type' or 'custom-rules' option")
            ("random", po::value<int>(), "create and solve a random solitaire "
                    "deal based on a seed. Must supply either 'random',"
                    "'solvability', 'benchmark' or list of deals to be solved.")
            ("classify", "outputs a simple 'solvable/not solvable' "
                    "classification")
            ("shortest-sols", "for each instance returns the shortest possible "
                    "solution. cannot be supplied alongside '--solvability'")
            ("solvability", "calculates the solvability "
                    "percentage of the supplied solitaire game. Must supply "
                    "either 'random', 'benchmark', 'solvability' or list of deals to be "
                    "solved.")
            ("benchmark", "outputs performance statistics for the solver on the "
                    "supplied solitaire game. Must supply "
                    "either 'random', 'benchmark', 'solvability' or list of deals to be "
                    "solved.");

    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
            ("input-files", po::value<vector<string>>(),
             "the file containing the deal to be solved");

    cmdline_options.add(main_options).add(hidden_options);

    p.add("input-files", -1);
}

// Returns true if we can continue solving the supplied solitaire(s)
bool command_line_helper::parse(int argc, const char* argv[]) {
    po::variables_map vm;
    // Attempts to parse cmdln options. Catches basic errors
    try {
        store(po::command_line_parser(argc, argv).options(cmdline_options)
                      .positional(p).run(), vm);
    } catch(po::error& e) {
        LOG_ERROR ("Error: " << e.what());
        return false;
    }

    // Updates the variables map which the user's options are stored in
    notify(vm);

    // Converts the variables map into flags and values

    help = (vm.count("help") != 0);

    available_game_types = (vm.count("available-game-types") != 0);

    if (vm.count("describe-game-rules")) {
        describe_game_rules = vm["describe-game-rules"].as<string>();
    }

    classify = (vm.count("classify") != 0);

    if (vm.count("input-files")) {
        input_files = vm["input-files"].as<vector<string>>();
    }

    if (vm.count("type")) {
        solitaire_type = vm["type"].as<string>();
    }

    if (vm.count("custom-rules")) {
        rules_file = vm["custom-rules"].as<string>();
    }

    if (vm.count("random")) {
        random_deal = vm["random"].as<int>();
    } else {
        random_deal = -1;
    }

    solvability = (vm.count("solvability") != 0);

    benchmark = (vm.count("benchmark") != 0);

    shortest_sols = (vm.count("shortest-sols") != 0);

    // Handle logic error scenarios
    return assess_errors();
}

bool command_line_helper::assess_errors() {
    if (help) {
        print_help();
        return false;
    }

    if (available_game_types || !describe_game_rules.empty()) return true;

    // The user must either supply input files, a random seed, or ask for the
    // solvability percentage, or benchmark
    int opt_count = (random_deal != -1) + !input_files.empty() + solvability + benchmark;

    if (opt_count > 1) {
        print_too_many_opts_error();
        return false;
    } else if (opt_count < 1) {
        print_no_opts_error();
        return false;
    }

    // The user must supply either a solitaire type or a rules file
    if ((solitaire_type.empty() && rules_file.empty())
            || (!solitaire_type.empty() && !rules_file.empty())) {
        print_sol_type_rules_error();
        return false;
    }

    if (shortest_sols && solvability) {
        print_shortest_sols_error();
        return false;
    }

    if (!solitaire_type.empty()) {
        return assess_sol_type();
    } else {
        return true;
    }
}

// Checks if the supplied solitaire type is in the list of valid solitaires
bool command_line_helper::assess_sol_type() {
    if (sol_preset_types::is_valid_preset(solitaire_type)) {
        return true;

    } else {
        LOG_ERROR ("Error: Solitaire type is not valid: " << solitaire_type);
        print_help();
        return false;
    }
}

void command_line_helper::print_help() {
    cerr <<  "Usage: solvitaire [options] input-file1 input-file2 ...\n"
            << main_options << "\n";
}

void command_line_helper::print_no_opts_error() {
    LOG_ERROR ("Error: User must supply input file(s), the '--random' "
            "option, the 'benchmark' option, or the '--solvability' option");
    print_help();
}

void command_line_helper::print_sol_type_rules_error() {
    LOG_ERROR ("Error: User must supply either a solitaire type, or a 'rules' "
                       "file");
    print_help();
}

void command_line_helper::print_too_many_opts_error() {
    LOG_ERROR ("Error: User must supply input file(s), the '--random' option, the 'benchmark' option,, "
                       "or the '--solvability' option, not multiple");
    print_help();
}

void command_line_helper::print_shortest_sols_error() {
    LOG_ERROR ("Error: User cannot supply '--shortest-sols' and '--solvability'");
    print_help();
}

const vector<string> command_line_helper::get_input_files() {
    return input_files;
}

const string command_line_helper::get_solitaire_type() {
    return solitaire_type;
}

const string command_line_helper::get_rules_file() {
    return rules_file;
}

int command_line_helper::get_random_deal() {
    return random_deal;
}

bool command_line_helper::get_help() {
    return help;
}

bool command_line_helper::get_classify() {
    return classify;
}

bool command_line_helper::get_solvability() {
    return solvability;
}

bool command_line_helper::get_available_game_types() {
    return available_game_types;
}

string command_line_helper::get_describe_game_rules() {
    return describe_game_rules;
}

bool command_line_helper::get_short_sols() {
    return shortest_sols;
}

bool command_line_helper::get_benchmark() {
    return benchmark;
}
