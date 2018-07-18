//
// Created by thecharlesblake on 10/21/17.
//

#ifndef SOLVITAIRE_DEAL_PARSER_H
#define SOLVITAIRE_DEAL_PARSER_H

#include <vector>

#include <boost/optional.hpp>

#include "../../../../../lib/rapidjson/document.h"
#include "../../../../../lib/rapidjson/schema.h"

class game_state;

class deal_parser {
public:
    static void parse(game_state&, const rapidjson::Document&);
    static void parse_tableau_piles(game_state&, const rapidjson::Document&);
    static void parse_hole(game_state&, const rapidjson::Document&);
    static void parse_cells(game_state&, const rapidjson::Document&);
    static void parse_stock(game_state&, const rapidjson::Document&);
    static void parse_waste(game_state&, const rapidjson::Document&);
    static void parse_reserve(game_state&, const rapidjson::Document&);
    static void parse_sequences(game_state&, const rapidjson::Document&);
    static void parse_accordion(game_state&, const rapidjson::Document&);
    static bool parse_foundations(game_state&, const rapidjson::Document&);
    static void fill_foundations(game_state&);
    static std::string deal_schema_json();
};

#endif //SOLVITAIRE_DEAL_PARSER_H
