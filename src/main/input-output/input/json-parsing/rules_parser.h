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
// Created by thecharlesblake on 1/3/18.
//

#ifndef SOLVITAIRE_RULES_PARSER_H
#define SOLVITAIRE_RULES_PARSER_H

#include "../../../game/sol_rules.h"

class rules_parser {
public:
    static const sol_rules from_file(std::string);
    static const sol_rules from_preset(std::string);
    static std::string rules_schema_json();

private:
    static sol_rules get_default();
    static void modify_sol_rules(sol_rules&, rapidjson::Document&);
};


#endif //SOLVITAIRE_RULES_PARSER_H
