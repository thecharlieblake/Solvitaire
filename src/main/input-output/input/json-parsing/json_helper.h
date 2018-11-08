/*
  Solvitaire: a solver for perfect information solitaire games
  Copyright (C) 2018 Charles Blake <thecharlesblake@live.co.uk> and 
  Ian Gent <Ian.Gent@st-andrews.ac.uk>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program (see LICENSE file); if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
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
