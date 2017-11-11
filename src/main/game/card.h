//
// Created by thecharlesblake on 10/11/17.
//

#ifndef SOLITAIRE_PIP_CARD_H
#define SOLITAIRE_PIP_CARD_H

#include <string>

class card {
public:
    card(int, int);
    card(const char*);
    int get_rank() const;
    char get_suit() const;
    int get_suit_val() const;

private:
    int rank;
    int suit;
};

std::ostream & operator<<(std::ostream &, card const &);
bool operator==(const card&, const card&);


#endif //SOLITAIRE_PIP_CARD_H
