//
// Created by thecharlesblake on 10/28/17.
//

#ifndef SOLVITAIRE_PILE_H
#define SOLVITAIRE_PILE_H

#include <vector>
#include <iterator>

#include "sol_rules.h"
#include "card.h"

class pile {
public:
    typedef uint8_t size_type;

    pile() = default;
    pile(std::vector<card>);
    pile(std::initializer_list<card>);

    // Inspect
    card top_card() const;
    bool empty() const;
    size_type size() const;

    // Modify
    void place(card);
    card take();

    // Indexes the vector, with the top card as index 0
    card& operator[] (size_type);
    card operator[] (size_type) const;

    friend bool operator==(const pile&, const pile&);
    friend std::size_t hash_value(pile const&);
private:
    // Underlying vector
    std::vector<card> pile_vec;
};


#endif //SOLVITAIRE_PILE_H
