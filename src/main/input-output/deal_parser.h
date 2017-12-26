//
// Created by thecharlesblake on 10/21/17.
//

#ifndef SOLVITAIRE_DEAL_PARSER_H
#define SOLVITAIRE_DEAL_PARSER_H

#include <vector>

#include <boost/optional.hpp>
#include <rapidjson/document.h>
#include <rapidjson/schema.h>

#include "game/card.h"
#include "game/sol_rules.h"
#include "game/game_state.h"

class deal_parser {
public:
    explicit deal_parser(const sol_rules& rules);
    game_state parse(const std::string&) const;
private:
    const sol_rules& rules;

    static rapidjson::Document read_file(const std::string&);
};

#endif //SOLVITAIRE_DEAL_PARSER_H
