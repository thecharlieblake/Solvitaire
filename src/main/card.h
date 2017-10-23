//
// Created by thecharlesblake on 10/11/17.
//

#ifndef SOLITAIRE_PIP_CARD_H
#define SOLITAIRE_PIP_CARD_H

#include <string>

class card {
public:
    card(const char*);
    int get_rank() const;
    char get_suit() const;
private:
    int rank;
    char suit;
};

std::ostream & operator<<(std::ostream &, card const &);


#endif //SOLITAIRE_PIP_CARD_H
