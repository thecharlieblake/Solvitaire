//
// Created by thecharlesblake on 10/11/17.
//

#ifndef SOLITAIRE_PIP_CARD_H
#define SOLITAIRE_PIP_CARD_H

#include <string>
#include <map>

class card {
public:
    // A card can either be created using two int values (rank & suit), or
    // using a string (e.g. "AS")
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
