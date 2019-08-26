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
// Created by thecharlesblake on 1/12/18.
//

#include "sol_rules.h"

typedef sol_rules::build_policy pol;
typedef sol_rules::spaces_policy s_pol;
typedef sol_rules::stock_deal_type sdt;
typedef sol_rules::direction dir;
typedef sol_rules::face_up_policy fu;
typedef sol_rules::built_group_type bgt;
typedef sol_rules::foundations_init_type fit;

// Note that the 'default' sol rules are set up by parsing the default json
// rules description. This initialisation is really just for testing purposes.
sol_rules::sol_rules() :
        tableau_pile_count(0),
        build_pol(pol::ANY_SUIT),
        spaces_pol(s_pol::ANY),
        move_built_group(bgt::NO),
        built_group_pol(pol::ANY_SUIT),
        two_decks(false),
        max_rank(13),
        hole(false),
        hole_base("AS"),
        hole_build_loops(true),
        foundations_present(false),
        foundations_init_cards(fit::NONE),
        foundations_base(1),
        foundations_only_comp_piles(false),
        diagonal_deal(false),
        cells(0),
        cells_pre_filled(0),
        stock_size(0),
        stock_deal_t(sdt::TABLEAU_PILES),
        stock_deal_count(1),
        reserve_size(0),
        reserve_stacked(false),
        face_up(fu::ALL),
        sequence_count(0),
        sequence_direction(dir::LEFT),
        sequence_build_pol(pol::SAME_SUIT),
        accordion_size(0) {
}
