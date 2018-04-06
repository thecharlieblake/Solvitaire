//
// Created by thecharlesblake on 11/14/17.
//

#include <iostream>
#include "sol_preset_types.h"
using namespace std;

string sol_preset_types::get(const string& s) {
    return mp[s];
}

bool sol_preset_types::is_valid_preset(const string& s) {
    return mp.count(s) > 0;
}

map<string, string> sol_preset_types::mp = {
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
  "stock deal count": 1,
  "stock redeal": false,
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
    "build policy": "red-black",
    "move built group": true
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
    "build policy": "red-black",
    "move built group": true
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

        },

        // --- END OF 'CANONICAL' GAMES ---

        {
                "raglan",

                R"(
{
  "tableau piles": {
    "count": 9,
    "build policy": "red-black",
    "diagonal deal": true
  },
  "reserve size": 6,
  "foundations removable": true,
  "foundations initial card": true
}
)"

        },

        {
                "klondike-deal-1",

                R"(
{
  "tableau piles": {
    "count": 7,
    "build policy": "red-black",
    "spaces policy": "kings",
    "move built group": true,
    "diagonal deal": true
  },
  "stock size": 24,
  "stock deal count": 1,
  "stock redeal": false
}
)"

        },

        {
                "klondike",

                R"(
{
  "tableau piles": {
    "count": 7,
    "build policy": "red-black",
    "spaces policy": "kings",
    "move built group": true,
    "diagonal deal": true
  },
  "stock size": 24,
  "stock deal count": 3,
  "stock redeal": true
}
)"

        },
        {
                "alina",

                R"(
{
  "tableau piles": {
    "count": 13,
    "build policy": "any-suit",
    "spaces policy": "no-build"
  },
  "foundations removable": true,
  "cells": 1
}
)"

        },
        {
                "castles-of-spain",

                R"(
{
  "tableau piles": {
    "count": 13,
    "build policy": "red-black"
  },
  "foundations removable": true
}
)"

        },
        {
                "delta-star",

                R"(
{
  "tableau piles": {
    "count": 12,
    "build policy": "same-suit"
  },
  "foundations initial card": true
}
)"

        },
        {
                "martha",

                R"(
{
  "tableau piles": {
    "count": 12,
    "build policy": "red-black"
  },
  "foundations removable": true,
  "foundations initial card": true
}
)"

        },
        {
                "one-cell",

                R"(
{
  "tableau piles": {
    "count": 13,
    "build policy": "red-black"
  },
  "foundations removable": true,
  "foundations initial card": true,
  "cells": 1
}
)"

        },
        {
                "two-cell",

                R"(
{
  "tableau piles": {
    "count": 13,
    "build policy": "red-black"
  },
  "foundations removable": true,
  "foundations initial card": true,
  "cells": 2
}
)"

        },
        {
                "free-cell-3-cell",

                R"(
{
  "tableau piles": {
    "build policy": "red-black"
  },
  "cells": 3
}
)"

        },
        {
                "free-cell-2-cell",

                R"(
{
  "tableau piles": {
    "build policy": "red-black"
  },
  "cells": 2
}
)"

        },
        {
                "free-cell-1-cell",

                R"(
{
  "tableau piles": {
    "build policy": "red-black"
  },
  "cells": 1
}
)"

        },
        {
                "free-cell-0-cell",

                R"(
{
  "tableau piles": {
    "build policy": "red-black"
  },
  "cells": 1
}
)"

        },
        {
                "free-cell-7-pile",

                R"(
{
  "tableau piles": {
    "count": 7,
    "build policy": "red-black"
  },
  "cells": 4
}
)"

        },
        {
                "free-cell-6-pile",

                R"(
{
  "tableau piles": {
    "count": 6,
    "build policy": "red-black"
  },
  "cells": 4
}
)"

        },
        {
                "free-cell-5-pile",

                R"(
{
  "tableau piles": {
    "count": 5,
    "build policy": "red-black"
  },
  "cells": 4
}
)"

        },
        {
                "bakers-game",

                R"(
{
  "tableau piles": {
    "build policy": "same-suit",
    "move built group": true,
    "spaces policy": "kings"
  },
  "cells": 4
}
)"

        },
        {
                "eight-off",

                R"(
{
  "tableau piles": {
    "build policy": "same-suit",
    "spaces policy": "kings"
  },
  "cells": 8
}
)"

        },
        {
                "eight-off-any-card-spaces",

                R"(
{
  "tableau piles": {
    "build policy": "same-suit"
  },
  "cells": 8
}
)"

        },
        {
                "fore-cell",

                R"(
{
  "tableau piles": {
    "build policy": "red-black",
    "spaces policy": "kings"
  },
  "cells": 4
}
)"

        },
        {
                "penguin",
                R"(

{
  "tableau piles": {
    "count": 7,
    "build policy": "same-suit",
    "move built group": true,
    "spaces policy": "kings"
  },
  "cells": 7,
  "foundations initial card": true
}
)"

        },
        {
                "sea-towers",
                R"(

{
  "tableau piles": {
    "count": 10,
    "build policy": "same-suit",
    "move built group": true,
    "spaces policy": "kings"
  },
  "cells": 4
}
)"

        },
        {
                "duchess",
                R"(

{
  "tableau piles": {
    "count": 4,
    "build policy": "red-black",
    "move built group": true
  },
  "reserve size": 12,
  "foundations initial card": true,
  "stock size": 36
}
)"

        },
        {
                "chameleon",
                R"(

{
  "tableau piles": {
    "count": 3,
    "build policy": "any-suit",
    "move built group": true
  },
  "reserve size": 13,
  "reserve stacked": true,
  "stock size": 36
}
)"

        },
        {
                "fan",
                R"(

{
  "tableau piles": {
    "count": 18,
    "build policy": "same-suit",
    "spaces policy": "kings"
  },
  "foundations removable": true
}
)"

        },
        {
                "scotch-patience",
                R"(

{
  "tableau piles": {
    "count": 18,
    "build policy": "red-black",
    "spaces policy": "no-build"
  },
  "foundations removable": true
}
)"

        },
        {
                "blind-alleys",
                R"(

{
  "tableau piles": {
    "count": 6,
    "build policy": "red-black",
    "move built group": true
  },
  "stock size": 30,
  "foundations initial card": true,
  "foundations removable": true
}
)"

        },
        {
                "east-haven",
                R"(

{
  "tableau piles": {
    "count": 7,
    "build policy": "red-black",
    "move built group": true
  },
  "foundations removable": true,
  "stock size": 31,
  "stock deal type": "tableau piles"
}
)"

        },
        {
                "northwest-territory",
                R"(

{
  "tableau piles": {
    "build policy": "red-black",
    "move built group": true,
    "spaces policy": "kings",
    "diagonal deal": true
  },
  "foundations removable": true,
  "reserve size": 16
}
)"

        },
        {
                "king-albert",
                R"(

{
  "tableau piles": {
    "count": 9,
    "build policy": "red-black",
    "diagonal deal": true
  },
  "foundations removable": true,
  "reserve size": 7
}
)"

        },
        {
                "spiderette",

                R"(
{
  "tableau piles": {
    "count": 7,
    "build policy": "any-suit",
    "move built group": true,
    "move built group policy": "same-suit",
    "diagonal deal": true
  },
  "foundations complete piles": true,
  "stock size": 24,
  "stock deal type": "tableau piles"
}
)"

        },
        {
                "will-o-the-wisp",

                R"(
{
  "tableau piles": {
    "count": 7,
    "build policy": "any-suit",
    "move built group": true,
    "move built group policy": "same-suit"
  },
  "foundations complete piles": true,
  "stock size": 31,
  "stock deal type": "tableau piles"
}
)"

        }
};

void sol_preset_types::print_available_games() {
    cout << "The following game types can be supplied to solvitaire using the "
            "'--type [name of game]' option:\n\n";
    for (auto const& x : mp) {
        cout << x.first << "\n";
    }
    cout << "\nUse the '--describe-game-rules [name of game]' option for an "
            "overview of the rules of each game:\n";
}

void sol_preset_types::describe_game_rules(string game) {
    auto rules_it = mp.find(game);
    if (rules_it == end(mp)) {
        cout << "'" << game << "' is not a valid game type. "
                "Enter '--available-game-types' for a "
                "list of the games presets that can be solved\n";
    } else {
        cout << "The rules of '" << game << "' are as follows:"
             << rules_it->second;

        if (game != "default") {
            cout << "\nTo see the default values of the JSON fields not covered "
                    "here, enter '--describe-game-rules default'\n";
        }
    }
}
