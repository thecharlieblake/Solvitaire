//
// Created by thecharlesblake on 10/24/17.
//

#include <vector>
#include <ostream>
#include <numeric>
#include <random>
#include <algorithm>
#include <functional>

#include <rapidjson/document.h>

#include "game_state.h"
#include "pile.h"

using namespace rapidjson;
using namespace std;

typedef sol_rules::build_order ord;
typedef sol_rules::build_policy pol;

// Construct an initial game state from a JSON doc
game_state::game_state(const Document& doc)
        : rules("simple-black-hole"),
          hole(false, ord::BOTH, pol::ANY_SUIT, true, rules.max_rank) {
    // Construct tableau piles
    assert(doc.HasMember("tableau piles"));
    const Value& json_tab_piles = doc["tableau piles"];
    assert(json_tab_piles.IsArray());

    int max_rank = 1;
    for (auto& json_tab : json_tab_piles.GetArray()) {
        pile tableau_pile(true, rules.build_order, pol::ANY_SUIT, false);

        for (auto& json_card : json_tab.GetArray()) {
            card c(json_card.GetString());
            if (c.get_rank() > max_rank) max_rank = c.get_rank();
            tableau_pile.place(c);
        }

        tableau_piles.push_back(tableau_pile);
    }

    // Assign hole card
    assert(doc.HasMember("hole card"));
    const Value& json_h_card = doc["hole card"];
    assert(json_h_card.IsString());
    hole.place(json_h_card.GetString());
}

// Construct an initial game state from a seed
game_state::game_state(int seed, const sol_rules& rules)
        : rules(rules),
          hole(false, ord::BOTH, pol::ANY_SUIT, true, rules.max_rank) {
    vector<card> deck = shuffled_deck(seed, rules.max_rank);

    // Deal to the tableau piles (row-by-row)
    for (vector<card>::size_type i = 0; i < deck.size(); i++) {
        card c = deck[i];

        // If there is a hole card, don't deal the ace of spades
        if (rules.hole && c == "AS") {
            hole.place("AS");
            continue;
        }

        // For the first row we must create all the tableau pile vectors
        if (i / rules.tableau_pile_count == 0) {
            pile p(true, rules.build_order, pol::ANY_SUIT, false);
            tableau_piles.push_back(p);
        }

        // Add the randomly generated card to the tableau piles
        tableau_piles[i % rules.tableau_pile_count].place(c);
    }

    // If there are foundation piles, create the relevant pile vectors
    if (rules.foundations) {
        for (int i = 0; i < 4; i++) {
            pile p(true, ord::ASCENDING, pol(i), false);
            foundations.push_back(p);
        }
    }
}

// Generate a randomly ordered vector of cards
vector<card> game_state::shuffled_deck(int seed, int max_rank = 13) {
    vector<int*> v;
    for (int i = 0; i < max_rank * 4; i++) {
        v.push_back(new int(i));
    }

    // Randomly shuffle the pointers
    auto rng = std::default_random_engine(seed);
    std::shuffle(std::begin(v), std::end(v), rng);

    vector<card> deck;
    for (int *i : v) {
        int r = ((*i) % max_rank) + 1;
        int s = (*i) / max_rank;

        deck.push_back(card(r, s));
    }

    // release memory
    for (int i = 0; i < max_rank * 4; i++) {
        delete v[i];
    }

    return deck;
}

vector<game_state> game_state::get_next_legal_states() const {
    // Generates vectors of references to all the piles from which
    // cards can be removed, and to which they can be added
    vector<pile&> can_remove;
    vector<pile&> can_add;

    for (pile p : tableau_piles) {
        can_remove.push_back(p);
        if (rules.build_order != ord::NO_BUILD) {
            can_add.push_back(p);
        }
    }
    if (rules.foundations) {
        for (pile p : foundations) {
            can_remove.push_back(p);
            can_add.push_back(p);
        }
    }
    if (rules.hole) {
        can_add.push_back(hole);
    }

    // The next legal states
    vector<game_state> next;

    for (pile &rem_pile : can_remove) {
        if (rem_pile.empty()) continue;

        for (pile &add_pile : can_add) {
            if (&add_pile != &rem_pile
                && add_pile.can_place(rem_pile.top_card())) {

                move(rem_pile, add_pile); // remove
                game_state s = *this;
                next.push_back(s);
                move(add_pile, rem_pile); // restore
            }
        }
    }

    return next;
}



bool game_state::adjacent(const card& a, const card& b) const {
    int x = a.get_rank();
    int y = b.get_rank();

    return x == y + 1
           || x + 1 == y
           || (x == 1 && y == rules.max_rank)
           || (x == rules.max_rank && y == 1);
}

bool game_state::is_solved() const {
    for (auto p : tableau_piles) {
        if (!p.empty()) {
            return false;
        }
    }
    return true;
}

ostream& game_state::print(ostream& stream) const {
    const char *sep = "------------------------------\n";

    if (rules.foundations) {
        print_header(stream, "Foundations");
        print_foundations(stream);
    }
    if (rules.tableau_pile_count > 0) {
        print_header(stream, "Tableau Piles");
        print_tableau_piles(stream);
    }
    if (rules.hole) {
        print_header(stream, "Hole Card");
        print_hole(stream);
    }
    return stream << "///////////////////////////////////\n\n";
}

void game_state::print_header(ostream& stream, const char* header) const {
    stream << "--- " << header << " ";
    int pad = 20 - strlen(header);
    for (int i = 0; i < pad; i++) {
        stream << '-';
    }
    stream << "\n";
}

void game_state::print_foundations(ostream& stream) const {
    stream << foundations[0].top_card() << "\t"
           << foundations[1].top_card() << "\t"
           << foundations[2].top_card() << "\t"
           << foundations[3].top_card() << "\n";
};

void game_state::print_tableau_piles(ostream& stream) const {
    bool empty_row = false;
    vector<card>::size_type row_idx = 0;

    while (!empty_row) {
        empty_row = true;

        for (pile tableau_pile : tableau_piles) {
            if (tableau_pile.size() > row_idx) {
                empty_row = false;
                stream << tableau_pile[row_idx];
            }
            stream << "\t";
        }

        stream << "\n";
        row_idx++;
    }
};

void game_state::print_hole(ostream& stream) const {
    stream << hole.top_card() << "\n";
};

ostream& operator <<(ostream& stream, const game_state& gs) {
    return gs.print(stream);
}