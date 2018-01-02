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
    // Factory methods
    static pile tableau_factory(sol_rules::build_order, sol_rules::build_policy,
                                sol_rules::spaces_policy);
    static pile cell_factory();
    static pile reserve_factory();
    static pile stock_factory();
    static pile waste_factory();
    static pile foundation_factory(sol_rules::build_policy);
    static pile hole_factory(int);

    // Constructors
    pile(sol_rules::build_order bo = sol_rules::build_order::DESCENDING,
         sol_rules::build_policy bp = sol_rules::build_policy::ANY_SUIT,
         sol_rules::spaces_policy sp = sol_rules::spaces_policy::ANY,
         bool removable = true,
         bool loops = false,
         int max_rank = 13);

    // Inspect
    bool can_place(const card) const;
    bool can_remove() const;
    card top_card() const;
    bool empty() const;
    uint8_t size() const;
    sol_rules::build_order get_build_order() const;

    // Modify
    void place(const card);
    card take();
    void clear();

    // Index like array
    card& operator[] (std::vector<card>::size_type);
    card operator[] (std::vector<card>::size_type) const;

    friend bool operator==(const pile&, const pile&);
    friend std::size_t hash_value(pile const&);

private:
    bool one_lt(int, int) const;

    // const pile properties
    const sol_rules::build_order build_order;
    const sol_rules::build_policy build_policy;
    const sol_rules::spaces_policy spaces_policy;
    const bool removable;
    const bool build_order_loops;
    const int max_rank;

    // Underlying vector
    std::vector<card> pile_vec;
};


#endif //SOLVITAIRE_PILE_H
