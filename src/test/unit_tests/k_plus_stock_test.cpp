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
#include "../../main/game/sol_rules.h"

typedef sol_rules::build_policy pol;
typedef sol_rules::stock_deal_type sdt;

using std::vector;
using std::ostream;
using std::initializer_list;


TEST(KPlusStock, NoRedealExact) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = false;
    sr.stock_deal_count = 3;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"2H","2D","3S","AD","2S","3C","AC","AH","AS"},
                    {},
                    {"3D"},
                    {"3H"},
                    {"2C"}
            },
            {
                    move(move::mtype::stock_k_plus, 0, 4, 3),
                    move(move::mtype::stock_k_plus, 0, 3, 9),
            },
            move(move::mtype::stock_k_plus, 0, 3, 9),
            0, 8,
            card(), "2D"
    );
}

TEST(KPlusStock, NoRedealOneExtra) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = false;
    sr.stock_deal_count = 3;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"2H","AD","2S","3C","AC","AH","AS"},
                    {},
                    {"3D"},
                    {"3H"},
                    {"2C"}
            },
            {
                    move(move::mtype::stock_k_plus, 0, 4, 3),
                    move(move::mtype::stock_k_plus, 0, 3, 7),
            },
            move(move::mtype::stock_k_plus, 0, 3, 7),
            0, 6,
            card(), "AD"
    );
}

TEST(KPlusStock, NoRedealTwoExtra) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = false;
    sr.stock_deal_count = 3;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"2S","2H","AD","2D","3C","AC","AH","AS"},
                    {},
                    {"3D"},
                    {"3H"},
                    {"2C"}
            },
            {
                    move(move::mtype::stock_k_plus, 0, 4, 3)
            },
            move(move::mtype::stock_k_plus, 0, 4, 3),
            5, 2,
            "3C", "AH"
    );
}

TEST(KPlusStock, RedealExact) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = true;
    sr.stock_deal_count = 3;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"2H","2D","3S","AD","2S","3C","AC","AH","AS"},
                    {},
                    {"3D"},
                    {"3H"},
                    {"2C"}
            },
            {
                    move(move::mtype::stock_k_plus, 0, 4, 3, false, false),
                    move(move::mtype::stock_k_plus, 0, 3, 9, false, true),
            },
            move(move::mtype::stock_k_plus, 0, 3, 9, false, true),
            8, 0,
            "AS", card()
    );
}

TEST(KPlusStock, RedealOneExtra) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = true;
    sr.stock_deal_count = 3;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"2H","AD","2S","3C","AC","AH","AS"},
                    {},
                    {"3D"},
                    {"3H"},
                    {"2C"}
            },
            {
                    move(move::mtype::stock_k_plus, 0, 4, 3, false, false),
                    move(move::mtype::stock_k_plus, 0, 3, 7, false, true),
            },
            move(move::mtype::stock_k_plus, 0, 3, 7, false, true),
            6, 0,
            "AS", card()
    );
}

TEST(KPlusStock, RedealTwoExtra) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = true;
    sr.stock_deal_count = 3;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"2S","2H","AD","2D","3C","AC","AH","AS"},
                    {},
                    {"3D"},
                    {"3H"},
                    {"2C"}
            },
            {
                    move(move::mtype::stock_k_plus, 0, 4, 3, false, false)
            },
            move(move::mtype::stock_k_plus, 0, 4, 3, false, false),
            5, 2,
            "3C", "AH"
    );
}

TEST(KPlusStock, RedealKPlus3Waste) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = true;
    sr.stock_deal_count = 3;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"2H","2D","3S","AD","2S","3C"},
                    {"AS","AH","AC"},
                    {"3D"},
                    {"3H"},
                    {"2C"}
            },
            {
                    move(move::mtype::stock_k_plus, 0, 3, 6, false, true),
                    move(move::mtype::stock_k_plus, 0, 4, 0, false, false)
            },
            move(move::mtype::stock_k_plus, 0, 3, 6, false, true),
            8, 0,
            "AS", card()
    );
}

TEST(KPlusStock, RedealKPlus4Waste) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = true;
    sr.stock_deal_count = 3;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"2H","AD","3S","2D","2S"},
                    {"AS","AH","AC","3C"},
                    {"3D"},
                    {"3H"},
                    {"2C"}
            },
            {
                    move(move::mtype::stock_k_plus, 0, 3, 5, false, true),
                    move(move::mtype::stock_k_plus, 0, 4, -1, false, false),
                    move(move::mtype::stock_k_plus, 0, 2, 2, false, false)
            },
            move(move::mtype::stock_k_plus, 0, 2, 2, false, false),
            3, 5,
            "3S", "2S"
    );
}

TEST(KPlusStock, RedealKPlus5Waste) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 10;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = true;
    sr.stock_deal_count = 3;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"2S","2D","2H","AD","2S"},
                    {"AS","AH","AC","3C","4S"},
                    {"3D"},
                    {"3H"},
                    {"2C"}
            },
            {
                    move(move::mtype::stock_k_plus, 0, 3, 3, false, false),
                    move(move::mtype::stock_k_plus, 0, 4, -2, false, false),
                    move(move::mtype::stock_k_plus, 0, 2, 4, false, false)
            },
            move(move::mtype::stock_k_plus, 0, 3, 3, false, false),
            2, 7,
            "2D", "AD"
    );
}

TEST(KPlusStock, RedealKPlusEmptyStock) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = true;
    sr.stock_deal_count = 3;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"2H","2D","3S","AD","2S","3C"},
                    {"AS","AH","AC"},
                    {"3D"},
                    {"3H"},
                    {"2C"}
            },
            {
                    move(move::mtype::stock_k_plus, 0, 3, 6, false, true),
                    move(move::mtype::stock_k_plus, 0, 4, 0, false, false)
            },
            move(move::mtype::stock_k_plus, 0, 3, 6, false, true),
            8, 0,
            "AS", card()
    );
}

TEST(KPlusStock, NoRedealCellFoundationMoves) {
    sol_rules sr;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = false;
    sr.stock_deal_count = 3;
    sr.cells = 2;
    sr.foundations_present = true;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"AC"}, {}, {}, {},
                    {"2D"}, {},
                    {"2H","AS","2C","AH","2S","3C"},
                    {"3S"}
            },
            {
                    // Cell moves
                    move(move::mtype::stock_k_plus, 6, 5, 3),
                    move(move::mtype::stock_k_plus, 6, 5, 6),

                    // Foundation moves
                    move(move::mtype::stock_k_plus, 6, 1, 3),

                    // Regular waste move
                    move(move::mtype::stock_k_plus, 6, 5, 0)
            },
            move(move::mtype::stock_k_plus, 6, 1, 3),
            3, 3,
            "2C", "2S"
    );
}

TEST(KPlusStock, RedealCellFoundationMoves) {
    sol_rules sr;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = true;
    sr.stock_deal_count = 3;
    sr.cells = 2;
    sr.foundations_present = true;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"AC"}, {}, {}, {},
                    {"2D"}, {},
                    {"2H","AS","2C","AH","2S","3C"},
                    {"3S"}
            },
            {
                    // Cell moves
                    move(move::mtype::stock_k_plus, 6, 5, 3, false, false),
                    move(move::mtype::stock_k_plus, 6, 5, 6, false, true),
                    move(move::mtype::stock_k_plus, 6, 5, 2, false, false),
                    move(move::mtype::stock_k_plus, 6, 5, 5, false, false),

                    // Foundation moves
                    move(move::mtype::stock_k_plus, 6, 1, 3, false, false),
                    move(move::mtype::stock_k_plus, 6, 2, 5, false, false),

                    // Regular waste move
                    move(move::mtype::stock_k_plus, 6, 5, 0)
            },
            move(move::mtype::stock_k_plus, 6, 2, 5, false, false),
            1, 5,
            "2H", "2C"
    );
}

TEST(KPlusStock, RedealCount1BottomStock) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = true;
    sr.stock_deal_count = 1;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"2H","2S","AD","2D"},
                    {},
                    {"3D"},
                    {"3H"},
                    {"2C"}
            },
            {
                    move(move::mtype::stock_k_plus, 0, 2, 1, false, false),
                    move(move::mtype::stock_k_plus, 0, 3, 4, false, true)
            },
            move(move::mtype::stock_k_plus, 0, 3, 4, false, true),
            3, 0,
            "2D", card()
    );
}

TEST(KPlusStock, RedealCount1TopStock) {
    sol_rules sr;
    sr.tableau_pile_count = 3;
    sr.build_pol = pol::SAME_SUIT;
    sr.stock_size = 9;
    sr.stock_deal_t = sdt::WASTE;
    sr.stock_redeal = true;
    sr.stock_deal_count = 1;

    test_helper::k_plus_stock_test(
            sr,
            {
                    {"2H","2S","AD","2D"},
                    {},
                    {"3D"},
                    {"3H"},
                    {"2C"}
            },
            {
                    move(move::mtype::stock_k_plus, 0, 2, 1, false, false),
                    move(move::mtype::stock_k_plus, 0, 3, 4, false, true)
            },
            move(move::mtype::stock_k_plus, 0, 2, 1, false, false),
            3, 0,
            "AD", card()
    );
}
