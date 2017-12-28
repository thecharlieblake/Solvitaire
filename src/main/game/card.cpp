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

suit_t card::suit_from_str(const char* c) {
    switch(tolower(c[strlen(c) - 1])) {
        case 'c': return suit_t::Clubs;
        case 'd': return suit_t::Diamonds;
        case 'h': return suit_t::Hearts;
        case 's': return suit_t::Spades;
        default: assert(false);
            return suit_t::Clubs;
    }
}

rank_t card::rank_from_str(const char* c) {
    switch(tolower(c[0])) {
        case 'a': return 1;
        case 'j': return 11;
        case 'q': return 12;
        case 'k': return 13;
        default: return static_cast<rank_t>(stoi(c));
    }
}

rank_t card::get_rank() const {
    return rank;
}

suit_t card::get_suit() const {
    return suit;
}

bool operator==(const card& l, const card& r) {
    return l.get_rank() == r.get_rank() && l.get_suit() == r.get_suit();
}

std::ostream & operator<<(std::ostream & s, card const & c) {
    switch(c.rank) {
        case 1 : s << "A";
        case 11: s << "J";
        case 12: s << "Q";
        case 13: s << "K";
        default: s << c.rank;
    }
    switch(c.suit) {
        case suit_t::Spades  : s << "S";
        case suit_t::Hearts  : s << "H";
        case suit_t::Clubs   : s << "C";
        case suit_t::Diamonds: s << "D";
    }
    return s;
}

size_t hash_value(card const& c) {
    boost::hash<unsigned char> hasher;
    unsigned char raw_val = static_cast<unsigned char>(
            static_cast<std::underlying_type_t<card::suit_t>>(c.get_suit())
            * 13 + c.get_rank());

    return hasher(raw_val);
}