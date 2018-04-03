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
    "build policy": "any-suit",
    "spaces policy": "any",
    "diagonal deal": false,
    "move built group": false,
    "move built group policy": "same-as-build"
  },
  "max rank": 13,
  "two decks": false,
  "hole": false,
  "foundations": true,
  "foundations initial card": false,
  "foundations removable": false,
  "foundations complete piles": false,
  "cells": 0,
  "stock size": 0,
  "stock deal type": "waste",
  "reserve size": 0,
  "reserve stacked": false
}
)"

        },
        {
                "spanish-patience",

                R"(
{
  "tableau piles": {
    "count": 13
  },
  "foundations removable": true
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
  "max rank": 2,
  "foundations removable": true
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
  },
  "foundations removable": true
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
  "max rank": 3,
  "foundations removable": true
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
                "fortunes-favor",

                R"(
{
  "tableau piles": {
    "count": 12,
    "build policy": "same-suit"
  },
  "foundations initial card": true,
  "stock size": 36
}
)"

        },
        {
                "simple-fortunes-favor",

                R"(
{
  "tableau piles": {
    "count": 3,
    "build policy": "same-suit"
  },
  "max rank": 4,
  "foundations initial card": true,
  "stock size": 3
}
)"

        },
        {
                "flower-garden",

                R"(
{
  "tableau piles": {
    "count": 6
  },
  "reserve size": 16,
  "foundations removable": true
}
)"

        },
        {
                "simple-flower-garden",

                R"(
{
  "tableau piles": {
    "count": 3
  },
  "max rank": 3,
  "reserve size": 3,
  "foundations removable": true
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
  "stock size": 34,
  "reserve size": 14,
  "reserve stacked": true
}
)"

        },
        {
                "simple-canfield",

                R"(
{
  "tableau piles": {
    "count": 3,
    "build policy": "red-black"
  },
  "max rank": 3,
  "stock size": 1,
  "reserve size": 2,
  "reserve stacked": true
}
)"

        },
        {
                "somerset",

                R"(
{
  "tableau piles": {
    "count": 10,
    "build policy": "red-black",
    "diagonal deal": true
  },
  "foundations removable": true
}
)"

        },
        {
                "simple-somerset",

                R"(
{
  "tableau piles": {
    "count": 4,
    "build policy": "red-black",
    "diagonal deal": true
  },
  "max rank": 2,
  "foundations removable": true
}
)"

        },
        {
                "alpha-star",

                R"(
{
  "tableau piles": {
    "count": 12,
    "build policy": "same-suit",
    "move built group": true
  },
  "foundations initial card": true
}
)"

        },
        {
                "simple-alpha-star",

                R"(
{
  "tableau piles": {
    "count": 3,
    "build policy": "same-suit",
    "move built group": true
  },
  "max rank": 5,
  "foundations initial card": true
}
)"

        },
        {
                "spider",

                R"(
{
  "tableau piles": {
    "count": 10,
    "build policy": "any-suit",
    "move built group": true,
    "move built group policy": "same-suit"
  },
  "foundations complete piles": true,
  "two decks": true,
  "stock size": 50,
  "stock deal type": "tableau piles"
}
)"

        },
        {
                "simple-spider",

                R"(
{
  "tableau piles": {
    "count": 4,
    "build policy": "any-suit",
    "move built group": true,
    "move built group policy": "same-suit"
  },
  "foundations complete piles": true,
  "two decks": true,
  "max rank": 3,
  "stock size": 12,
  "stock deal type": "tableau piles"
}
)"

        }
};