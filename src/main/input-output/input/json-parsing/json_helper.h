//
// Created by thecharlesblake on 11/13/17.
//

#ifndef SOLVITAIRE_UTIL_H
#define SOLVITAIRE_UTIL_H

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../../../../../lib/rapidjson/schema.h"
#include "../../../game/search-state/game_state.h"

class json_helper {
public:
    static rapidjson::Document get_file_json(const std::string&);
    static void json_parse_warning(const std::string& msg);
    static void json_parse_err(const std::string& msg);
    static const std::string schema_err_str(const rapidjson::SchemaValidator&);
    static void print_game_state_as_json(const game_state&);
private:
    static boost::property_tree::ptree piles_to_ptree(const game_state&, const std::list<pile::ref>&);
    static boost::property_tree::ptree piles_to_ptree(const game_state&, const std::vector<pile::ref>&);
    static boost::property_tree::ptree pile_to_ptree(const pile&);
    static boost::property_tree::ptree card_to_ptree(const card&);
};


#endif //SOLVITAIRE_UTIL_H
