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
