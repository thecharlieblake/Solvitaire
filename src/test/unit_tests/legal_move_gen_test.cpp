//
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>

#include "../test_helper.h"
#include "../../main/game/game_state.h"
#include "../../main/game/sol_rules.h"

typedef sol_rules::build_policy pol;
typedef sol_rules::spaces_policy s_pol;
typedef sol_rules::stock_deal_type sdt;
typedef game_state::move mv;

using namespace std;

bool moves_eq(vector<mv>&, vector<mv>&);
ostream& operator <<(ostream&, const mv&);
ostream& operator <<(ostream&, const vector<mv>&);

TEST(LegalMoveGen, BuildPolAnySuit) {
    sol_rules sr;
    sr.build_pol = pol::ANY_SUIT;
    sr.tableau_pile_count = 5;

    game_state gs(sr, initializer_list<pile>{
            {"2C"},
            {"2H"},
            {"2D"},
            {"2S"},
            {"AC"}
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
            mv(4,0),
            mv(4,1),
            mv(4,2),
            mv(4,3)
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, BuildPolRedBlack) {
    sol_rules sr;
    sr.build_pol = pol::RED_BLACK;
    sr.tableau_pile_count = 5;

    game_state gs(sr, initializer_list<pile>{
            {"2C"},
            {"2H"},
            {"2S"},
            {"2D"},
            {"AC"}
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
            mv(4,1),
            mv(4,3)
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, BuildPolSameSuit) {
    sol_rules sr;
    sr.build_pol = pol::SAME_SUIT;
    sr.tableau_pile_count = 5;

    game_state gs(sr, initializer_list<pile>{
            {"2C"},
            {"2H"},
            {"2S"},
            {"2D"},
            {"AC"}
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
            mv(4,0)
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, BuildPolNoBuild) {
    sol_rules sr;
    sr.build_pol = pol::NO_BUILD;
    sr.tableau_pile_count = 5;

    game_state gs(sr, initializer_list<pile>{
            {"2C"},
            {"2H"},
            {"2S"},
            {"2D"},
            {"AC"}
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, SpacesPolAny) {
    sol_rules sr;
    sr.spaces_pol = s_pol::ANY;
    sr.tableau_pile_count = 2;

    game_state gs(sr, initializer_list<pile>{
            {},
            {"AC"}
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
            mv(1, 0)
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, SpacesPolKings) {
    sol_rules sr;
    sr.spaces_pol = s_pol::KINGS;
    sr.tableau_pile_count = 3;

    game_state gs(sr, initializer_list<pile>{
            {},
            {"AC"},
            {"KD"}
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
            mv(2, 0)
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, SpacesPolNoBuild) {
    sol_rules sr;
    sr.spaces_pol = s_pol::NO_BUILD;
    sr.tableau_pile_count = 2;

    game_state gs(sr, initializer_list<pile>{
            {},
            {"AC"}
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, MoveBuiltGroupTrue) {
    sol_rules sr;
    sr.move_built_group = true;
    sr.tableau_pile_count = 2;

    game_state gs(sr, initializer_list<pile>{
            {},
            {"2C", "AC"}
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
            mv(1, 0, 1),
            mv(1, 0, 2)
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, MoveBuiltGroupFalse) {
    sol_rules sr;
    sr.move_built_group = false;
    sr.tableau_pile_count = 2;

    game_state gs(sr, initializer_list<pile>{
            {},
            {"AC", "2C"}
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
            mv(1, 0, 1)
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, Foundations) {
    sol_rules sr;
    sr.foundations = true;
    sr.tableau_pile_count = 4;

    game_state gs(sr, initializer_list<pile>{
            {}, {}, {}, {},
            {"AC"},
            {"AH"},
            {"AS"},
            {"AD"},
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
            mv(4, 0),
            mv(5, 1),
            mv(6, 2),
            mv(7, 3)
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, FoundationsRemovable) {
    sol_rules sr;
    sr.foundations = true;
    sr.foundations_removable = true;
    sr.tableau_pile_count = 1;

    game_state gs(sr, initializer_list<pile>{
            {"AC","2C","3C"}, // Without a few cards here dominance logic blocks
            {},               // removing from foundation
            {},
            {},
            {}
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
            mv(0, 4)
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, Cells) {
    sol_rules sr;
    sr.cells = 2;
    sr.tableau_pile_count = 1;

    game_state gs(sr, initializer_list<pile>{
            {"3D"}, {}, // The cells
            {"4H"}
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
            mv(0, 1),
            mv(0, 2),
            mv(2, 1)
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, StockDealToWaste) {
    sol_rules sr;
    sr.foundations = true;
    sr.tableau_pile_count = 3;
    sr.stock_size = 2;
    sr.stock_deal_t = sdt::WASTE;

    game_state gs(sr, initializer_list<pile>{
            {}, {}, {}, {}, // The foundations
            {"AC", "AD"}, // The stock
            {}, // The waste
            {}, {}, {} // The tableau piles
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
            mv(4, 5)
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}

TEST(LegalMoveGen, StockDealToTableau) {
    sol_rules sr;
    sr.foundations = true;
    sr.tableau_pile_count = 3;
    sr.stock_size = 2;
    sr.stock_deal_t = sdt::TABLEAU_PILES;

    game_state gs(sr, initializer_list<pile>{
            {}, {}, {}, {}, // The foundations
            {"AC", "AD"}, // The stock
            {"3H"}, {"5D"}, {"7C"} // The tableau piles
    });
    vector<mv> actual_moves = gs.get_legal_moves();

    vector<mv> exp_moves = {
            mv(4, 255, 2) // Special stock-to-tableau syntax
    };

    ASSERT_TRUE(moves_eq(exp_moves, actual_moves)) << actual_moves;
}



bool moves_eq(vector<mv>& exp_moves, vector<mv>& actual_moves) {
    if (exp_moves.size() != actual_moves.size()) return false;

    for (mv m : actual_moves) {
        auto it = find(begin(exp_moves), end(exp_moves), m);
        bool found = it != end(exp_moves);

        if (found) exp_moves.erase(it);
        else return false;
    }

    return true;
}

ostream& operator <<(ostream& os, const mv& m) {
    return os << "move:(" << int(m.from)
              << ","    << int(m.to)
              << ","     << int(m.count)
              << ")";
}

ostream& operator <<(ostream& os, const vector<mv>& moves) {
    for (const mv m : moves) {
        os << m << ",\n";
    }
    return os;
}
