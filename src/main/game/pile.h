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
    pile(bool, sol_rules::build_order, sol_rules::build_policy, bool, int m = 13);

    bool can_place(const card&) const;
    bool can_remove() const;
    card top_card() const;
    bool empty() const;
    unsigned long size() const;

    void place(const card&);
    card take();

    card& operator[] (std::vector<card>::size_type);
    card operator[] (std::vector<card>::size_type) const;

    friend bool operator==(const pile&, const pile&);
    friend std::ostream & operator<<(std::ostream &, pile const &);

private:
    bool one_lt(int, int) const;

    const bool remove;
    const sol_rules::build_order build_order;
    const sol_rules::build_policy build_policy;
    const bool build_order_loops;
    const int max_rank;

    std::vector<card> pile_vec;
};

void move(pile*, pile*);


#endif //SOLVITAIRE_PILE_H
