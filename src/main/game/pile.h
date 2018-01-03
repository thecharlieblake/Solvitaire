//
// Created by thecharlesblake on 10/28/17.
//

#ifndef SOLVITAIRE_PILE_H
#define SOLVITAIRE_PILE_H

#include <vector>

#include "sol_rules.h"
#include "card.h"

class pile {
public:
    // Inspect
    card top_card() const;
    bool empty() const;
    uint8_t size() const;

    // Modify
    void place(card);
    card take();
    void clear();

    // Index like array
    card& operator[] (std::vector<card>::size_type);
    card operator[] (std::vector<card>::size_type) const;

    friend bool operator==(const pile&, const pile&);
    friend std::size_t hash_value(pile const&);
private:
    // Underlying vector
    std::vector<card> pile_vec;
};


#endif //SOLVITAIRE_PILE_H
