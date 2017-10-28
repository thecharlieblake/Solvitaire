//
// Created by thecharlesblake on 10/28/17.
//

#ifndef SOLVITAIRE_PILE_H
#define SOLVITAIRE_PILE_H

#import "sol_rules.h"

class pile {
public:
    pile();
    pile(bool, sol_rules::build_order, sol_rules::build_policy, bool, int m = 13);
    bool can_place(const card&) const;
    void place(const card&);
    card take();
    card top_card() const;
    bool can_remove() const;
    bool empty() const;
    int size() const;
    card& operator[] (std::vector::size_type);
    card operator[] (std::vector::size_type) const;
private:
    vector<card> pile_vec;
    const bool can_remove;
    const bool build_order_loops;
    const int max_rank;
    const sol_rules::build_order build_order;
    const sol_rules::build_policy build_policy;
    bool one_lt(int, int);
};

void move(const pile&, const pile&);


#endif //SOLVITAIRE_PILE_H
