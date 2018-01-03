//
// Created by thecharlesblake on 1/3/18.
//

#ifndef SOLVITAIRE_RULES_PARSER_H
#define SOLVITAIRE_RULES_PARSER_H

#include "../../../game/sol_rules.h"

class rules_parser {
public:
    static const sol_rules from_file(std::string);
    static const sol_rules from_preset(std::string);

private:
    static sol_rules get_default();
    static void modify_sol_rules(sol_rules&, rapidjson::Document&);
};


#endif //SOLVITAIRE_RULES_PARSER_H
