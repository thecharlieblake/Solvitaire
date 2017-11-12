//
// Created by thecharlesblake on 10/28/17.
//

#include <vector>
#include <ostream>

#include "pile.h"

using namespace std;

pile::pile(bool r, sol_rules::build_order bo, sol_rules::build_policy bp,
           bool l, int mr) :
    remove(r),
    build_order(bo),
    build_policy(bp),
    build_order_loops(l),
    max_rank(mr) {}

bool pile::can_place(const card& c) const {
    typedef sol_rules::build_order ord;
    typedef sol_rules::build_policy pol;

    if (build_order == ord::NO_BUILD) {
        return false;
    } else if (build_order == ord::SINGLE_CARD) {
        return empty();
    }

    if (empty()) {
        if (sol_rules::is_suit(build_policy)) {
            return c.get_suit_val() == sol_rules::suit_val(build_policy)
                   && c.get_rank() == 1;
        } else {
            return true;
        }
    }

    card top_c = top_card();

    // Check suit
    if (build_policy == pol::SAME_SUIT && c.get_suit_val()
                                          != top_c.get_suit_val()) {
        return false;
    }
    if (sol_rules::is_suit(build_policy) && c.get_suit_val()
                                     != sol_rules::suit_val(build_policy)) {
        return false;
    }

    int tc_r = top_c.get_rank();
    int comp_r = c.get_rank();

    if (build_order == ord::ASCENDING) {
        return one_lt(tc_r, comp_r);
    } else if (build_order == ord::DESCENDING) {
        return one_lt(comp_r, tc_r);
    } else if (build_order == ord::BOTH) {
        return one_lt(tc_r, comp_r) || one_lt(comp_r, tc_r);
    } else {
        return false;
    }
}

bool pile::one_lt(int a, int b) const {
    return (a == b - 1) || (build_order_loops && a == max_rank && b == 1);
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
    return remove;
}

bool pile::empty() const {
    return pile_vec.empty();
}

unsigned long pile::size() const {
    return pile_vec.size();
}

card& pile::operator[] (vector<card>::size_type i) {
    return pile_vec[i];
}
card pile::operator[] (vector<card>::size_type i) const {
    return pile_vec[i];
}

void move(pile *a, pile *b) {
    b->place(a->take());
}

bool operator==(const pile& a, const pile& b) {
    return a.pile_vec == b.pile_vec;
}

ostream & operator<<(ostream & stream, pile const & p) {
    if (p.empty()) {
        return stream << "[]";
    } else {
        return stream << p.top_card();
    }
}
