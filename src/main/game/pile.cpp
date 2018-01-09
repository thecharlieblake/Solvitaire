//
// Created by thecharlesblake on 10/28/17.
//

#include <vector>
#include <ostream>

#include <boost/functional/hash.hpp>

#include "pile.h"

using namespace std;
using namespace boost;

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
    card c = top_card();
    pile_vec.pop_back();
    return c;
}

bool operator==(const pile& a, const pile& b) {
    return a.pile_vec == b.pile_vec;
}

size_t hash_value(pile const& p) {
    return hash_range(begin(p.pile_vec), end(p.pile_vec));
}
