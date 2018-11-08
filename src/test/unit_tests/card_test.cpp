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
#include "../../main/game/card.h"

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
