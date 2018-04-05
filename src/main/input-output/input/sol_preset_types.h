//
// Created by thecharlesblake on 11/14/17.
//

#ifndef SOLVITAIRE_SOL_PRESET_TYPES_H
#define SOLVITAIRE_SOL_PRESET_TYPES_H

#include <string>
#include <map>

class sol_preset_types {
public:
    static std::string get(const std::string& s);
    static bool is_valid_preset(const std::string& s);
    static void print_available_games();
    static void describe_game_rules(std::string);

private:
    static std::map<std::string, std::string> mp;
};

#endif //SOLVITAIRE_SOL_PRESET_TYPES_H
