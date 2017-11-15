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
    "build policy": "any-suit"
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
                "black-hole",

                R"(
{
  "tableau piles": {
    "count": 17,
    "build order": "no-build"
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
    "count": 9,
    "build order": "no-build"
  },
  "max rank": 7,
  "hole": true,
  "foundations": false
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
  "max rank": 3
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
    "count": 3,
    "build policy": "red-black"
  },
  "max rank": 3,
  "cells": 1
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