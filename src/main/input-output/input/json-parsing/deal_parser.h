//
// Created by thecharlesblake on 10/21/17.
//

#ifndef SOLVITAIRE_DEAL_PARSER_H
#define SOLVITAIRE_DEAL_PARSER_H

#include <vector>

#include <boost/optional.hpp>
#include <rapidjson/document.h>
#include <rapidjson/schema.h>

#include "../../../game/sol_rules.h"

class game_state;

class deal_parser {
public:
    static void parse(game_state&, const rapidjson::Document&);
private:
    static void parse_tableau_piles(game_state&, const rapidjson::Document&);
    static void parse_hole(game_state&, const rapidjson::Document&);
    static void parse_cells(game_state&, const rapidjson::Document&);
    static void parse_stock(game_state&, const rapidjson::Document&);
    static void parse_reserve(game_state&, const rapidjson::Document&);
    static void fill_foundations(game_state&);
};

#endif //SOLVITAIRE_DEAL_PARSER_H
