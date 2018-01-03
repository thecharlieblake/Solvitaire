//
// Created by thecharlesblake on 11/14/17.
//

#include "sol_preset_types.h"
using namespace std;

string sol_preset_types::get(const string& s) {
    return mp[s];
}

bool sol_preset_types::is_valid_preset(const string& s) {
    return mp.count(s) > 0;
}

std::map<string, string> sol_preset_types::mp = {
        {
                "default",

                R"(
{
  "tableau piles": {
    "count": 8,
    "build order": "descending",
    "build policy": "any-suit",
    "spaces policy": "any"
  },
  "max rank": 13,
  "hole": false,
  "foundations": true,
  "cells": 0,
  "reserve size": 0,
  "stock size": 0
}
)"

        },
        {
                "spanish-patience",

                R"(
{
  "tableau piles": {
    "count": 13
  }
}
)"

        },
        {
                "simple-spanish-patience",

                R"(
{
  "tableau piles": {
    "count": 3
  },
  "max rank": 2
}
)"

        },
        {
                "free-cell",

                R"(
{
  "tableau piles": {
    "build policy": "red-black"
  },
  "cells": 4
}
)"

        },
        {
                "simple-free-cell",

                R"(
{
  "tableau piles": {
    "count": 4,
    "build policy": "red-black"
  },
  "max rank": 4,
  "cells": 1
}
)"

        },
        {
                "bakers-dozen",

                R"(
{
  "tableau piles": {
    "count": 13,
    "spaces policy": "no-build"
  }
}
)"

        },
        {
                "simple-bakers-dozen",

                R"(
{
  "tableau piles": {
    "count": 3,
    "spaces policy": "no-build"
  },
  "max rank": 3
}
)"

        },
        {
                "black-hole",

                R"(
{
  "tableau piles": {
    "count": 17,
    "build policy": "no-build"
  },
  "hole": true,
  "foundations": false
}
)"

        },
        {
                "simple-black-hole",

                R"(
{
  "tableau piles": {
    "count": 4,
    "build policy": "no-build"
  },
  "max rank": 5,
  "hole": true,
  "foundations": false
}
)"

        },
        {
                "canfield",

                R"(
{
  "tableau piles": {
    "count": 4,
    "build policy": "red-black"
  },
  "reserve size": 14,
  "stock size": 34
}
)"

        },
        {
                "simple-canfield",

                R"(
{
  "tableau piles": {
    "count": 2,
    "build policy": "red-black"
  },
  "max rank": 2,
  "reserve size": 2,
  "stock size": 2
}
)"

        }
};