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

card::suit_t card::suit_from_str(const char* c) {
    switch(tolower(c[strlen(c) - 1])) {
        case 'c': return suit_t::Clubs;
        case 'd': return suit_t::Diamonds;
        case 'h': return suit_t::Hearts;
        case 's': return suit_t::Spades;
        default: assert(false);
            return suit_t::Clubs;
    }
}

card::rank_t card::rank_from_str(const char* c) {
    switch(tolower(c[0])) {
        case 'a': return 1;
        case 'j': return 11;
        case 'q': return 12;
        case 'k': return 13;
        default: return static_cast<rank_t>(stoi(c));
    }
}

card::suit_t card::get_suit() const {
    return suit;
}

card::colour_t card::get_colour() const {
    switch (suit) {
        case card::suit_t::Spades  :
        case card::suit_t::Clubs   : return card::colour_t::Black;
        default :                    return card::colour_t::Red;
    }
}

card::rank_t card::get_rank() const {
    return rank;
}

bool operator==(const card& l, const card& r) {
    return l.get_rank() == r.get_rank() && l.get_suit() == r.get_suit();
}

std::ostream & operator<<(std::ostream & s, card const & c) {
    switch(c.get_rank()) {
        case 1 : s << "A"; break;
        case 11: s << "J"; break;
        case 12: s << "Q"; break;
        case 13: s << "K"; break;
        default: s << int(c.get_rank());
    }
    switch(c.get_suit()) {
        case card::suit_t::Spades  : s << "S"; break;
        case card::suit_t::Hearts  : s << "H"; break;
        case card::suit_t::Clubs   : s << "C"; break;
        case card::suit_t::Diamonds: s << "D"; break;
    }
    return s;
}

size_t hash_value(card const& c) {
    boost::hash<unsigned char> hasher;
    auto raw_val = static_cast<unsigned char>(
            static_cast<std::underlying_type_t<card::suit_t>>(c.get_suit())
            * 13 + c.get_rank());

    return hasher(raw_val);
}