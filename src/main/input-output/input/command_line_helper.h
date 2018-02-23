//
// Created by thecharlesblake on 10/16/17.
//

#ifndef SOLVITAIRE_COMMAND_LINE_HELPER_H
#define SOLVITAIRE_COMMAND_LINE_HELPER_H

#include <boost/program_options.hpp>

class command_line_helper {
public:
    command_line_helper();

    bool parse(int argc, const char* argv[]);
    const std::vector<std::string> get_input_files();
    const std::string get_solitaire_type();
    const std::string get_rules_file();
    int get_random_deal();
    bool get_help();
    bool get_classify();

private:
    bool assess_errors();
    bool assess_sol_type();
    void print_help();
    void print_no_input_error();
    void print_sol_type_rules_error();
    void print_rand_plus_input_err();

    boost::program_options::options_description cmdline_options;
    boost::program_options::options_description main_options;
    boost::program_options::positional_options_description p;

    std::vector<std::string> input_files;
    std::string solitaire_type;
    std::string rules_file;
    int random_deal;
    bool help;
    bool classify;
};

#endif //SOLVITAIRE_COMMAND_LINE_HELPER_H
