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
// Created by thecharlesblake on 20/06/18.
//

#include <cmath>
#include <algorithm>
#include "binomial_ci.h"

using std::pow;
using std::min;
using std::max;
using std::pair;

pair<double, double> binomial_ci::wilson(int s_, int f_) {
    auto s = static_cast<double>(s_);
    auto f = static_cast<double>(f_);
    double z = 1.96;
    double z2 = pow(z, 2);
    double n = s + f;
    double p = (s + (z2 / 2.0)) / (n + z2);
    double half_ci = (z / (n + z2)) * sqrt(((s * f) / n) + (z2 / 4.0));
    return { p - half_ci
           , p + half_ci };
}

pair<double, double> binomial_ci::wilson(int s, int f, int i) {
    pair<double, double> a = wilson(s, f + i);
    pair<double, double> b = wilson(s + i, f);
    return {a.first, b.second};
}

pair<double, double> binomial_ci::agresti_coull(int s, int f) {
    double z = 1.96;
    double n_ = s + f + pow(z, 2);
    double p = (s + pow(z, 2)/2)/n_;
    double half_ci = z * sqrt((p - pow(p, 2))/n_);
    return { max(0.0, p - half_ci)
            , min(1.0, p + half_ci) };
}

pair<double, double> binomial_ci::agresti_coull(int s, int f, int i) {
    pair<double, double> a = agresti_coull(s, f + i);
    pair<double, double> b = agresti_coull(s + i, f);
    return {a.first, b.second};
}
