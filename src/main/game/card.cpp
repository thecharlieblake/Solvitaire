//
// Created by thecharlesblake on 10/11/17.
//

#include <sstream>

#include <boost/functional/hash.hpp>

#include "card.h"

using namespace std;
using namespace boost;

// A card can either be created using two int values (rank & suit), or
// using a string (e.g. "AS")

card::card(suit_t s, rank_t r) : suit(s), rank(r) {}

card::card(const char* c) : suit(suit_from_str(c)), rank(rank_from_str(c)) {}

card::card() : card(0, 0) {}

card::suit_t card::suit_from_str(const char* c) {
    switch(tolower(c[strlen(c) - 1])) {
        case 'c': return suit::Clubs;
        case 'h': return suit::Hearts;
        case 's': return suit::Spades;
        case 'd': return suit::Diamonds;
        default: assert(false);
            return suit::Clubs;
    }
}

card::rank_t card::rank_from_str(const char* c) {
    switch(tolower(c[0])) {
        case 'a': return 1;
        case 'j': return 11;
        case 'q': return 12;
        case 'k': return 13;
        default:
            assert(stoi(c) >= 1 && stoi(c) <= 13);
            return static_cast<rank_t>(stoi(c));
    }
}

card::suit_t card::get_suit() const {
    return suit;
}

card::colour_t card::get_colour() const {
    switch (suit) {
        case card::suit::Spades  :
        case card::suit::Clubs   : return card::colour::Black;
        default :                    return card::colour::Red;
    }
}

card::rank_t card::get_rank() const {
    return rank;
}

std::string card::to_string() const {
    string s;
    switch(rank) {
        case 1 : s += "A"; break;
        case 11: s += "J"; break;
        case 12: s += "Q"; break;
        case 13: s += "K"; break;
        default: s += std::to_string(rank);
    }
    switch(suit) {
        case card::suit::Spades  : s += "S"; break;
        case card::suit::Hearts  : s += "H"; break;
        case card::suit::Clubs   : s += "C"; break;
        case card::suit::Diamonds: s += "D"; break;
    }
    return s;
}

const card card::divider = card(0, 0);

bool operator==(const card& l, const card& r) {
    return l.get_rank() == r.get_rank() && l.get_suit() == r.get_suit();
}

bool operator!=(const card& l, const card& r) {
    return !(l == r);
}

bool operator<(const card& l, const card& r) {
    if (l.get_rank() == r.get_rank())
        return l.get_suit() < r.get_suit();
    else
        return l.get_rank() < r.get_rank();
}

bool operator>(const card& l, const card& r) {
    if (l.get_rank() == r.get_rank())
        return l.get_suit() > r.get_suit();
    else
        return l.get_rank() > r.get_rank();
}

bool operator<=(const card& l, const card& r) {
    if (l.get_rank() == r.get_rank())
        return l.get_suit() <= r.get_suit();
    else
        return l.get_rank() <= r.get_rank();
}

bool operator>=(const card& l, const card& r) {
    if (l.get_rank() == r.get_rank())
        return l.get_suit() >= r.get_suit();
    else
        return l.get_rank() >= r.get_rank();
}
