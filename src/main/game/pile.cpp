//
// Created by thecharlesblake on 10/28/17.
//

#include <vector>
#include <ostream>

#include "pile.h"

using namespace std;

typedef sol_rules::build_order ord;
typedef sol_rules::build_policy pol;

////////////
// Static //
////////////

pile pile::foundation_factory(pol p) {
    return pile(true, ord::ASCENDING, p, false);
}
pile pile::cell_factory() {
    return pile(true, ord::SINGLE_CARD, pol::ANY_SUIT, false);
}
pile pile::tableau_factory(ord o) {
    return pile(true, o, pol::ANY_SUIT, false);
}
pile pile::reserve_factory() {
    return pile(true, ord::NO_BUILD, pol::ANY_SUIT, false);
}
pile pile::stock_factory() {
    return pile(true, ord::NO_BUILD, pol::ANY_SUIT, false);
}
pile pile::waste_factory() {
    return pile(true, ord::NO_BUILD, pol::ANY_SUIT, false);
}
pile pile::hole_factory(int max_rank) {
    return pile(false, ord::BOTH, pol::ANY_SUIT, true, max_rank);
}

////////////////
// Non-static //
////////////////

pile::pile(bool r, ord bo, pol bp, bool l, int mr) :
    remove(r),
    build_order(bo),
    build_policy(bp),
    build_order_loops(l),
    max_rank(mr) {}

bool pile::can_place(const card& c) const {

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



card pile::top_card() const {
    return pile_vec.back();
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

ord pile::get_build_order() const {
    return build_order;
}

card& pile::operator[] (vector<card>::size_type i) {
    return pile_vec[i];
}
card pile::operator[] (vector<card>::size_type i) const {
    return pile_vec[i];
}


void pile::place(const card& c) {
    pile_vec.push_back(c);
}

card pile::take() {
    card c = top_card();
    pile_vec.pop_back();
    return c;
}

void pile::clear() {
    pile_vec.clear();
}

bool operator==(const pile& a, const pile& b) {
    return a.pile_vec == b.pile_vec;
}
