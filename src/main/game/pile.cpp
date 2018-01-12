//
// Created by thecharlesblake on 10/28/17.
//

#include <vector>
#include <ostream>

#include "pile.h"

using namespace std;

pile::pile(std::vector<card> pv) : pile_vec(pv) {}

card pile::top_card() const {
    assert(!empty());
    return pile_vec.back();
}

bool pile::empty() const {
    return pile_vec.empty();
}

pile::size_type pile::size() const {
    return static_cast<uint8_t>(pile_vec.size());
}

card& pile::operator[] (size_type i) {
    return pile_vec[pile_vec.size() - 1 - i];
}

card pile::operator[] (size_type i) const {
    return pile_vec[pile_vec.size() - 1 - i];
}

void pile::place(const card c) {
    pile_vec.emplace_back(c);
}

card pile::take() {
    assert(!empty());
    card c = top_card();
    pile_vec.pop_back();
    return c;
}

bool operator==(const pile& a, const pile& b) {
    return a.pile_vec == b.pile_vec;
}
