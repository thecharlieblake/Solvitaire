/*
  Solvitaire: a solver for perfect information solitaire games
  Copyright (C) 2018 Charles Blake <thecharlesblake@live.co.uk> and 
  Ian Gent <Ian.Gent@st-andrews.ac.uk>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program (see LICENSE file); if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
//
// Created by thecharlesblake on 10/28/17.
//

#include <vector>
#include <ostream>

#include "pile.h"

using namespace std;

const pile::size_type pile::max_size_type = 255;

pile::pile(std::vector<card> pv) : pile_vec(pv) {
}

pile::pile(std::initializer_list<card> il) : pile_vec(il) {
}

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
    vector<card>::size_type s = pile_vec.size() - 1 - i;
    assert(s < pile_vec.size());
    return pile_vec[s];
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

bool operator!=(const pile& a, const pile& b) {
    return a.pile_vec != b.pile_vec;
}

bool operator<(const pile& a, const pile& b) {
    if (a.size() == b.size()) {
        return a.pile_vec < b.pile_vec;
    } else {
        return a.size() < b.size();
    }
}

bool operator>(const pile& a, const pile& b) {
    if (a.size() == b.size()) {
        return a.pile_vec > b.pile_vec;
    } else {
        return a.size() > b.size();
    }
}

bool operator<=(const pile& a, const pile& b) {
    if (a.size() == b.size()) {
        return a.pile_vec <= b.pile_vec;
    } else {
        return a.size() <= b.size();
    }
}

bool operator>=(const pile& a, const pile& b) {
    if (a.size() == b.size()) {
        return a.pile_vec >= b.pile_vec;
    } else {
        return a.size() >= b.size();
    }
}
