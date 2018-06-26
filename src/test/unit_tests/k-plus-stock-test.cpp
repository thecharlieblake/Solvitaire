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
                    move(move::mtype::stock_to_waste, 0, 4, 3),
                    move(move::mtype::stock_to_waste, 0, 3, 9),
            },
            move(move::mtype::stock_to_waste, 0, 3, 9),
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
                    move(move::mtype::stock_to_waste, 0, 4, 3),
                    move(move::mtype::stock_to_waste, 0, 3, 7),
            },
            move(move::mtype::stock_to_waste, 0, 3, 7),
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
                    move(move::mtype::stock_to_waste, 0, 4, 3)
            },
            move(move::mtype::stock_to_waste, 0, 4, 3),
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
                    move(move::mtype::stock_to_waste, 0, 4, 3),
                    move(move::mtype::stock_to_waste, 0, 3, 9),
            },
            move(move::mtype::stock_to_waste, 0, 3, 9),
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
                    move(move::mtype::stock_to_waste, 0, 4, 3),
                    move(move::mtype::stock_to_waste, 0, 3, 7),
            },
            move(move::mtype::stock_to_waste, 0, 3, 7),
            0, 6,
            card(), "AD"
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
                    move(move::mtype::stock_to_waste, 0, 4, 3)
            },
            move(move::mtype::stock_to_waste, 0, 4, 3),
            8, 0,
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
                    move(move::mtype::stock_to_waste, 0, 3, 9),
                    move(move::mtype::stock_to_waste, 0, 4, 3)
            },
            move(move::mtype::stock_to_waste, 0, 4, 3),
            6, 2,
            "3C", "AH"
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
                    move(move::mtype::stock_to_waste, 0, 3, 9),
                    move(move::mtype::stock_to_waste, 0, 4, 3),
                    move(move::mtype::stock_to_waste, 0, 2, 6)
            },
            move(move::mtype::stock_to_waste, 0, 2, 6),
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
                    move(move::mtype::stock_to_waste, 0, 3, 8),
                    move(move::mtype::stock_to_waste, 0, 4, 3),
                    move(move::mtype::stock_to_waste, 0, 2, 9)
            },
            move(move::mtype::stock_to_waste, 0, 3, 8),
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
                    move(move::mtype::stock_to_waste, 0, 3, 9),
                    move(move::mtype::stock_to_waste, 0, 4, 3)
            },
            move(move::mtype::stock_to_waste, 0, 3, 9),
            8, 0,
            "AS", card()
    );
}
