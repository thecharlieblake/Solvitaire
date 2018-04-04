//
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>

#include "test_helper.h"
#include "../main/game/card.h"

TEST(Card, Rank) {
    for (card::rank_t r = 1; r <= 13; r++) {
        card::suit_t s = card::suit::Clubs;
        card c(s, r);
        ASSERT_EQ(r, c.get_rank());
    }
}

TEST(Card, Suit) {
    for (card::suit_t s : {
            card::suit::Clubs,
            card::suit::Hearts,
            card::suit::Spades,
            card::suit::Diamonds
    }) {
        card::rank_t r = 1;
        card c(s, r);
        ASSERT_EQ(s, c.get_suit());
    }
}

TEST(Card, Colour) {
    card::rank_t r = 1;

    for (card::suit_t s : {
            card::suit::Clubs,
            card::suit::Spades
    }) {
        card c(s, r);
        ASSERT_EQ(card::colour::Black, c.get_colour());
    }

    for (card::suit_t s : {
            card::suit::Hearts,
            card::suit::Diamonds
    }) {
        card c(s, r);
        ASSERT_EQ(card::colour::Red, c.get_colour());
    }
}

TEST(Card, EqOps) {
    for (card::rank_t r = 1; r <= 13; r++) {
        for (card::suit_t s : {
                card::suit::Clubs,
                card::suit::Hearts,
                card::suit::Spades,
                card::suit::Diamonds
        }) {
            card a(s, r);
            card b(s, r);
            ASSERT_TRUE (a == b);
            ASSERT_FALSE(a != b);
        }
    }
}

TEST(Card, RelativeOps) {
    for (card::rank_t r = 1; r <= 12; r++) {
        for (card::suit_t s : {
                card::suit::Clubs,
                card::suit::Hearts,
                card::suit::Spades,
                card::suit::Diamonds
        }) {
            card a(s, r);
            card b(s, card::rank_t(r+1));
            ASSERT_TRUE (a < b);
            ASSERT_TRUE (a <= b);
            ASSERT_FALSE(a > b);
            ASSERT_FALSE(a >= b);
        }
    }
}

TEST(Card, StringConstructor) {
    ASSERT_EQ(card(card::suit::Spades  , card::rank_t(1) ), card("AS" ));
    ASSERT_EQ(card(card::suit::Clubs   , card::rank_t(1) ), card("Ac" ));
    ASSERT_EQ(card(card::suit::Diamonds, card::rank_t(5) ), card("5D" ));
    ASSERT_EQ(card(card::suit::Hearts  , card::rank_t(11)), card("11H"));
    ASSERT_EQ(card(card::suit::Spades  , card::rank_t(11)), card("JS" ));
    ASSERT_EQ(card(card::suit::Clubs   , card::rank_t(13)), card("kc" ));
}
