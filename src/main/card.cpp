//
// Created by thecharlesblake on 10/11/17.
//

#include <ctype.h>

#include "card.h"

using namespace std;

card::card() {}

card::card(const char* c) {
    switch(tolower(c[0])) {
        case 'a': rank = 1;
                  break;
        case 'j': rank = 11;
                  break;
        case 'q': rank = 12;
                  break;
        case 'k': rank = 13;
                  break;
        default: rank = c[0] - '0';
    }

    suit = c[1];
}

int card::get_rank() const {
    return rank;
}

char card::get_suit() const {
    return suit;
}

std::ostream & operator<<(std::ostream & Str, card const & c) {
    std::string r;
    if (c.get_rank() == 1) {
        r = "A";
    } else if (c.get_rank() <= 10) {
        r = std::to_string(c.get_rank());
    } else if (c.get_rank() == 11) {
        r = "J";
    } else if (c.get_rank() == 12) {
        r = "Q";
    } else if (c.get_rank() == 13) {
        r = "K";
    }

    r += c.get_suit();

    return Str << r;
}