//
// Created by thecharlesblake on 10/11/17.
//

#ifndef SOLITAIRE_PIP_CARD_H
#define SOLITAIRE_PIP_CARD_H

#include <string>

struct Card {
    Card(const int, const std::string);
    const int rank;
    const std::string suit;
};

std::ostream & operator<<(std::ostream &, Card const &);


#endif //SOLITAIRE_PIP_CARD_H
