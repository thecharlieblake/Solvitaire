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
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>

#include "../test_helper.h"
#include "../../main/game/search-state/game_state.h"
#include "../../main/game/global_cache.h"

typedef sol_rules::build_policy pol;

TEST(FoundationsDominance, SameSuit) {
    test_helper::run_foundations_dominance_test(pol::SAME_SUIT, {
            "AC","2C","3C","4C","AH","2H","3H","4H","AS","2S","3S","4S","AD",
            "2D","3D","4D"});
}

TEST(FoundationsDominance, RedBlack) {
    test_helper::run_foundations_dominance_test(pol::RED_BLACK, {
            "AC","2C","AH","2H","AS","3H","2S","AD","3C","4H","3S","2D","4C",
            "4S","3D","4D"});
}

TEST(FoundationsDominance, AnySuit) {
    test_helper::run_foundations_dominance_test(pol::ANY_SUIT, {
            "AC","2C","AH","2H","AS","2S","AD","3C","3H","3S","2D","4C","4H",
            "4S","3D","4D"});
}