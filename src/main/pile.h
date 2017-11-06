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
    pile();
    pile(bool, sol_rules::build_order, sol_rules::build_policy, bool, int m = 13);

    void set_remove(bool);
    void set_loops(bool);
    void set_max_rank(int);
    void set_build_order(sol_rules::build_order);
    void set_build_policy(sol_rules::build_policy);

    bool can_place(const card&) const;
    void place(const card&);
    card take();
    card top_card() const;
    bool can_remove() const;
    bool empty() const;
    unsigned long size() const;
    card& operator[] (std::vector<card>::size_type);
    card operator[] (std::vector<card>::size_type) const;

    friend bool operator==(const pile&, const pile&);
    friend std::ostream & operator<<(std::ostream &, pile const &);
private:
    std::vector<card> pile_vec;
    bool remove;
    bool build_order_loops;
    int max_rank;
    sol_rules::build_order build_order;
    sol_rules::build_policy build_policy;
    bool one_lt(int, int) const;
};

void move(pile*, pile*);


#endif //SOLVITAIRE_PILE_H
