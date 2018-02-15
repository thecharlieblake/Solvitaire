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
    friend class hasher;
    friend class game_state;
    friend class cached_game_state;
public:
    typedef uint8_t size_type;
    const static size_type max_size_type;

    pile() = default;
    pile(std::vector<card>);
    pile(std::initializer_list<card>);

    // Inspect
    card top_card() const;
    bool empty() const;
    size_type size() const;

    // Indexes the vector, with the top card as index 0
    card& operator[] (size_type);
    card operator[] (size_type) const;

    friend bool operator==(const pile&, const pile&);
    friend bool operator<(const pile&, const pile&);
    friend bool operator>(const pile&, const pile&);
    friend bool operator<=(const pile&, const pile&);
    friend bool operator>=(const pile&, const pile&);
private:
    // Modify
    void place(card);
    card take();

    // Underlying vector
    std::vector<card> pile_vec;
};


#endif //SOLVITAIRE_PILE_H
