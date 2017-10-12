//
// Created by thecharlesblake on 10/11/17.
//

#include "Card.h"
#include <string>

Card::Card(const int r, const std::string s): rank(r), suit(s) {}

std::ostream & operator<<(std::ostream & Str, Card const & c) {
    std::string r;
    if (c.rank == 1) {
        r = "A";
    } else if (c.rank <= 10) {
        r = std::to_string(c.rank);
    } else if (c.rank == 11) {
        r = "J";
    } else if (c.rank == 12) {
        r = "Q";
    } else if (c.rank == 13) {
        r = "K";
    }

    return Str << r << c.suit;
}