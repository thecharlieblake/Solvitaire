//
// Created by thecharlesblake on 10/11/17.
//

#include <sstream>
#include <ctype.h>

#include "card.h"

using namespace std;

card::card() : rank(-1), suit(-1) {}

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
        default: rank = atoi(c);
    }

    char s;
    if (rank >= 10 && !letter) {
        s = c[2];
    } else {
        s = c[1];
    }

    s = tolower(s);

    if (s == 's') {
        suit = 0;
    } else if (s == 'h') {
        suit = 1;
    } else if (s == 'd') {
        suit = 2;
    } else if (s == 'c') {
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
        case 0: return 'S';
        case 1: return 'H';
        case 2: return 'D';
        case 3: return 'C';
        default: return 'X';
    }
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