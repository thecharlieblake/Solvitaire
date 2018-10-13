//
// Created by thecharlesblake on 10/16/17.
//

#ifndef SOLVITAIRE_COMMAND_LINE_HELPER_H
#define SOLVITAIRE_COMMAND_LINE_HELPER_H

#include <boost/program_options.hpp>
#include "../../game/search-state/game_state.h"

class command_line_helper {
public:
    command_line_helper();
    enum class streamliner_opt {NONE, AUTO_FOUNDATIONS, SUIT_SYMMETRY, BOTH, SMART};

    bool parse(int argc, const char* argv[]);
    const std::vector<std::string> get_input_files();
    const std::string get_solitaire_type();
    const std::string get_rules_file();
    int get_random_deal();
    bool get_classify();
    bool get_deal_only();
    int get_solvability();
    uint get_cores();
    bool get_available_game_types();
    bool get_benchmark();
    streamliner_opt get_streamliners();
    game_state::streamliner_options get_streamliners_game_state();
    std::vector<int> get_resume();
    uint64_t get_cache_capacity();
    std::string get_describe_game_rules();
    uint64_t get_timeout();
    static game_state::streamliner_options convert_streamliners(streamliner_opt);

private:
    bool assess_errors();
    bool assess_sol_type();
    void print_help();
    void print_sol_type_rules_error();
    void print_no_opts_error();
    void print_too_many_opts_error();
    void print_resume_error();
    void print_streamliner_error(const std::string&);

    boost::program_options::options_description cmdline_options;
    boost::program_options::options_description main_options;
    boost::program_options::positional_options_description p;

    std::vector<std::string> input_files;
    std::string solitaire_type;
    std::string rules_file;
    std::string describe_game_rules;
    int random_deal;
    bool help;
    bool classify;
    bool deal_only;
    int solvability;
    std::vector<int> resume;
    uint cores;
    bool available_game_types;
    bool benchmark;
    streamliner_opt streamliners;
    uint64_t cache_capacity;
    uint64_t timeout;
};

#endif //SOLVITAIRE_COMMAND_LINE_HELPER_H
