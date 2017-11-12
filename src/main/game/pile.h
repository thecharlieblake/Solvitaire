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
    static pile foundation_factory(sol_rules::build_policy);
    static pile cell_factory();
    static pile tableau_factory(sol_rules::build_order);
    static pile reserve_factory();
    static pile stock_factory();
    static pile waste_factory();
    static pile hole_factory(int max_rank = 13);

    pile(bool, sol_rules::build_order, sol_rules::build_policy, bool,
         int max_rank = 13);

    bool can_place(const card&) const;
    bool can_remove() const;
    card top_card() const;
    bool empty() const;
    unsigned long size() const;

    void place(const card&);
    card take();
    void clear();

    card& operator[] (std::vector<card>::size_type);
    card operator[] (std::vector<card>::size_type) const;

    friend bool operator==(const pile&, const pile&);

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
