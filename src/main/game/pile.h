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

#ifndef SOLVITAIRE_PILE_H
#define SOLVITAIRE_PILE_H

#include <vector>
#include <iterator>

#include "sol_rules.h"
#include "card.h"

class pile {
    friend class hasher;
    friend class game_state;
    friend class cached_game_state;
public:
    typedef uint8_t size_type;
    const static size_type max_size_type;
    typedef uint8_t ref;

    pile() = default;
    pile(std::vector<card>);
    pile(std::initializer_list<card>);

    // Inspect
    card top_card() const;
    bool empty() const;
    size_type size() const;

    // Indexes the vector, with the top card as index 0
    card& operator[] (size_type);
    card operator[] (size_type) const;

    friend bool operator==(const pile&, const pile&);
    friend bool operator!=(const pile&, const pile&);
    friend bool operator<(const pile&, const pile&);
    friend bool operator>(const pile&, const pile&);
    friend bool operator<=(const pile&, const pile&);
    friend bool operator>=(const pile&, const pile&);

    // Modify
    void place(card);
    card take();

private:
    // Underlying vector
    std::vector<card> pile_vec;
};


#endif //SOLVITAIRE_PILE_H
