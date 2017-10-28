//
// Created by thecharlesblake on 10/28/17.
//

#include "pile.h"
#include "sol_rules.h"
#include "card.h"

pile::pile(bool c, sol_rules::build_order bo, sol_rules::build_policy bp,
           bool l, int m) : can_remove(c), build_order(bo),
                                 build_policy(bp), build_order_loops(l),
                                 max_rank(m) {}

bool pile::can_place(const card& c) const {
    typedef sol_rules::build_order ord;
    typedef sol_rules::build_policy pol;

    if (build_order == ord::NO_BUILD) {
        return false;
    }

    if (empty()) {
        if (build_policy < 4) {
            return c.get_suit_val() == build_policy;
        } else {
            return true;
        }
    }

    card top_card = top_card();

    // Check suit
    if (build_policy == pol::SAME_SUIT && c.get_suit_val() != top_card.get_suit_val()) {
        return false;
    }
    if (build_policy < 4 && c.get_suit_val() != build_policy) {
        return false;
    }

    int tc_r = top_card.get_rank();
    int comp_r = c.get_rank();

    if (build_order == ord::ASCENDING) {
        return one_lt(tc_r, comp_r);
    } else if (build_order == ord::DESCENDING) {
        return one_lt(comp_r, tc_r);
    } else if (build_order == ord::BOTH) {
        return one_lt(tc_r, comp_r) || one_lt(comp_r, tc_r);
    }
}

bool pile::one_lt(int a, int b) {
    return (a == b - 1) || (loop && a == max_rank && b == 1);
}

void pile::place(const card& c) {
    pile_vec.push_back(c);
}

card pile::top_card() const {
    return pile_vec.back();
}

card pile::take() {
    card c = top_card();
    pile_vec.pop_back();
    return c;
}

bool pile::can_remove() const {
    return can_remove;
}

bool pile::empty() const {
    return pile_vec.empty();
}

int pile::size() const {
    return pile_vec.size();
}

card& pile::operator[] (std::vector::size_type i) {
    return pile_vec[i];
}
card pile::operator[] (std::vector::size_type i) const {
    return pile_vec[i];
}


void move(const pile& a, const pile& b) {
    b.place(a.take());
}
