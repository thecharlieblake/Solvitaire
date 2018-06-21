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
