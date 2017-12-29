//
// Created by thecharlesblake on 10/28/17.
//

#include <vector>
#include <ostream>

#include <boost/functional/hash.hpp>

#include "pile.h"

using namespace std;
using namespace boost;

typedef sol_rules::build_order ord;
typedef sol_rules::build_policy pol;

////////////
// Static //
////////////

pile pile::tableau_factory(ord o, pol p) {
    return pile(o, p);
}
pile pile::cell_factory() {
    return pile(ord::SINGLE_CARD, pol::N_A);
}
pile pile::reserve_factory() {
    return pile(ord::NO_BUILD, pol::N_A);
}
pile pile::stock_factory() {
    return pile(ord::NO_BUILD, pol::N_A);
}
pile pile::waste_factory() {
    return pile(ord::NO_BUILD, pol::N_A);
}
pile pile::foundation_factory(pol p) {
    return pile(ord::ASCENDING, p);
}
pile pile::hole_factory(int max_rank) {
    return pile(ord::BOTH, pol::ANY_SUIT, false, true, max_rank);
}

////////////////
// Non-static //
////////////////

pile::pile(ord bo, pol bp, bool r, bool l, int mr) :
        build_order(bo),
        build_policy(bp),
        removable(r),
        build_order_loops(l),
        max_rank(mr) {
}

bool pile::can_place(const card c) const {
    if (build_order == ord::ANY) {
        return true;
    } else if (build_order == ord::NO_BUILD) {
        return false;
    } else if (build_order == ord::SINGLE_CARD) {
        return empty();
    }

    if (empty()) {
        if (sol_rules::is_suit(build_policy)) {
            return c.get_suit() == sol_rules::suit_val(build_policy)
                   && c.get_rank() == 1;
        } else {
            return true;
        }
    }

    card top_c = top_card();

    // Check suit
    if (build_policy == pol::SAME_SUIT && c.get_suit() != top_c.get_suit()) {
        return false;
    }
    if (sol_rules::is_suit(build_policy) && c.get_suit()
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
    return removable;
}

bool pile::empty() const {
    return pile_vec.empty();
}

uint8_t pile::size() const {
    return static_cast<uint8_t>(pile_vec.size());
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


void pile::place(const card c) {
    pile_vec.emplace_back(c);
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

size_t hash_value(pile const& p) {
    return hash_range(begin(p.pile_vec), end(p.pile_vec));
}
