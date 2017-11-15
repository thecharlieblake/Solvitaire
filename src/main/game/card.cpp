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

card::card(int r, int s) : rank(r), suit(s) {}

card::card(const char* c) : rank(-1), suit(-1) {
    bool letter = false;
    switch(tolower(c[0])) {
        case 'a': rank = 1;
                  letter = true;
                  break;
        case 'j': rank = 11;
                  letter = true;
                  break;
        case 'q': rank = 12;
                  letter = true;
                  break;
        case 'k': rank = 13;
                  letter = true;
                  break;
        default: rank = stoi(c);
    }

    char s;
    if (rank >= 10 && !letter) {
        s = c[2];
    } else {
        s = c[1];
    }

    s = static_cast<char>(tolower(s));

    if (s == 'c') {
        suit = 0;
    } else if (s == 'd') {
        suit = 1;
    } else if (s == 'h') {
        suit = 2;
    } else if (s == 's') {
        suit = 3;
    }
}

bool operator==(const card& l, const card& r) {
    return l.get_rank() == r.get_rank() && l.get_suit() == r.get_suit();
}

int card::get_rank() const {
    return rank;
}

char card::get_suit() const {
    switch (suit) {
        case 0: return 'C';
        case 1: return 'D';
        case 2: return 'H';
        case 3: return 'S';
        default: return 'X';
    }
}

int card::get_suit_val() const {
    return suit;
}

std::ostream & operator<<(std::ostream & s, card const & c) {
    std::string r;
    if (c.get_rank() == 1) {
        s << "A";
    } else if (c.get_rank() <= 10) {
        s << c.get_rank();
    } else if (c.get_rank() == 11) {
        s << "J";
    } else if (c.get_rank() == 12) {
        s << "Q";
    } else if (c.get_rank() == 13) {
        s << "K";
    }

    s << c.get_suit();
    return s;
}

size_t hash_value(card const& c) {
    boost::hash<unsigned char> hasher;
    unsigned char raw_val = static_cast<unsigned char>(
            c.get_suit_val() * 13 + c.get_rank());

    return hasher(raw_val);
}