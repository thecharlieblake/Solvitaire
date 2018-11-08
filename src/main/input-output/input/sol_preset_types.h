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
