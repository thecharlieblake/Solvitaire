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
// Created by thecharlesblake on 10/24/17.
//

#ifndef SOLVITAIRE_RANDOM_H
#define SOLVITAIRE_RANDOM_H

#include <vector>
#include <list>
#include <string>
#include <random>
#include <functional>

#include <boost/functional/hash.hpp>
#include <boost/optional/optional.hpp>

#include "document.h"
#include "../card.h"
#include "../pile.h"
#include "../sol_rules.h"
#include "../move.h"

#define WIDTH 256	/* each RC4 output is 0 <= x < 256 */

typedef struct {
    int n;		/* Number of entries in the a array */
    int a[WIDTH];	/* A big array of integers */
} Key;

typedef struct {
    int S[WIDTH];
    int i;
    int j;
    int m;
} ARC4;

int lowbits(int n);
long getnext(ARC4 *me, int count);
ARC4 *makeARC4(Key *key);
void mixkey(char *seed, Key *key);
void randseed(char *seed);
double baurandom();
int randint(int n);
void set_seed(int seed);
int int_in_range_inclusive(int a, int b);

#endif //SOLVITAIRE_RANDOM_H
