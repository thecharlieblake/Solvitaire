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
// Created by thecharlesblake on 10/28/17.
//

#ifndef SOLVITAIRE_SOL_RULES_H
#define SOLVITAIRE_SOL_RULES_H

#include <string>
#include <vector>
#include <boost/optional.hpp>

#include "card.h"
#include "../../../lib/rapidjson/document.h"

struct sol_rules {
    sol_rules();
    
    // Tableau policies
    enum class build_policy {
        NO_BUILD,
        SAME_SUIT,
        RED_BLACK,
        ANY_SUIT
    };
    enum class spaces_policy {
        NO_BUILD,
        KINGS,
        ANY,
        AUTO_RESERVE_THEN_WASTE,
        AUTO_WASTE_THEN_STOCK
    };
    enum class accordion_policy {
        SAME_RANK,
        SAME_SUIT,
        RED_BLACK,
        ANY_SUIT
    };
    enum class stock_deal_type {
        WASTE,
        TABLEAU_PILES
    };
    enum class face_up_policy {
        ALL,
        TOP_CARDS
    };
    enum class direction {
        LEFT,
        RIGHT,
        BOTH
    };
    enum class built_group_type {
        YES,
        NO,
        WHOLE_PILE,
        MAXIMAL_GROUP
    };
    enum class foundations_init_type {
        NONE,
        ONE,
        ALL
    };

    uint8_t tableau_pile_count;
    build_policy build_pol;
    spaces_policy spaces_pol;
    built_group_type move_built_group;
    build_policy built_group_pol;
    bool two_decks;
    uint8_t max_rank;
    bool hole;
    bool foundations_present;
    foundations_init_type foundations_init_cards;
    boost::optional<card::rank_t> foundations_base;
    bool foundations_removable;
    bool foundations_only_comp_piles;
    bool diagonal_deal;
    uint8_t cells;
    uint8_t cells_pre_filled;
    uint8_t stock_size;
    stock_deal_type stock_deal_t;
    uint8_t stock_deal_count;
    bool stock_redeal;
    uint8_t reserve_size;
    bool reserve_stacked;
    face_up_policy face_up;
    uint8_t sequence_count;
    direction sequence_direction;
    build_policy sequence_build_pol;
    bool sequence_fixed_suit;
    uint8_t accordion_size;
    std::vector<std::pair<direction, uint8_t>> accordion_moves;
    std::vector<accordion_policy> accordion_pol;
};

#endif //SOLVITAIRE_SOL_RULES_H
