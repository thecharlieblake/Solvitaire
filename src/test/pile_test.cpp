//
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>

#include "test_helper.h"
#include "../main/game/pile.h"

TEST(Pile, Size) {
    pile p = {};
    ASSERT_EQ(0, p.size());

    p = {"AS"};
    ASSERT_EQ(1, p.size());

    p = {"AS", "KC", "4H"};
    ASSERT_EQ(3, p.size());
}

TEST(Pile, TopCard) {
    pile p = {"AS"};
    ASSERT_EQ(card("AS"), p.top_card());

    p = {"AS", "KC", "4H"};
    ASSERT_NE(card("AS"), p.top_card());
    ASSERT_EQ(card("4H"), p.top_card());
}

TEST(Pile, Index) {
    pile p = {"AS"};
    ASSERT_EQ(card("AS"), p[0]);

    p = {"AS", "KC", "4H"};
    ASSERT_EQ(card("AS"), p[2]);
    ASSERT_EQ(card("KC"), p[1]);
    ASSERT_EQ(card("4H"), p[0]);
}

TEST(Pile, Place) {
    pile t1 = {};
    pile t2 = {"AS"};
    pile t3 = {"AS", "KC", "4H"};

    pile p = {};
    ASSERT_EQ(p, t1);

    p.place("AS");
    ASSERT_NE(p, t1);
    ASSERT_EQ(p, t2);

    p.place("KC");
    p.place("4H");
    ASSERT_NE(p, t2);
    ASSERT_EQ(p, t3);
}

TEST(Pile, Take) {
    pile t1 = {};
    pile t2 = {"AS"};
    pile t3 = {"AS", "KC", "4H"};

    pile p = {"AS", "KC", "4H"};
    ASSERT_EQ(p, t3);

    p.take();
    p.take();
    ASSERT_NE(p, t3);
    ASSERT_EQ(p, t2);

    p.take();
    ASSERT_NE(p, t2);
    ASSERT_EQ(p, t1);
}

TEST(Pile, CompOperators) {
    pile p1 = {"3S"};
    pile p2 = {"AS", "AD", "AC"};
    pile p3 = {"2S", "2D", "2C"};
    pile p4 = {"AS", "AD", "AC", "AH"};

    ASSERT_LT(p1, p2);
    ASSERT_LT(p2, p3);
    ASSERT_LT(p3, p4);
    ASSERT_LT(p1, p4);
    ASSERT_EQ(p4, p4);
}
