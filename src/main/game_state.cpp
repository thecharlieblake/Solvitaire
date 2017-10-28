//
// Created by thecharlesblake on 10/24/17.
//

#include <vector>
#include <ostream>
#include <numeric>
#include <random>
#include <algorithm>

#include <rapidjson/document.h>

#include "game_state.h"

using namespace rapidjson;
using namespace std;

// Construct an initial game state from a JSON doc
game_state::game_state(const Document& doc) : rules("simple-black-hole") {
    // Construct tableau piles
    assert(doc.HasMember("tableau piles"));
    const Value& json_tab_piles = doc["tableau piles"];
    assert(json_tab_piles.IsArray());

    int max_rank = 1;
    for (auto& json_tab : json_tab_piles.GetArray()) {
        vector<card> tableau_pile;

        for (auto& json_card : json_tab.GetArray()) {
            card c(json_card.GetString());
            if (c.get_rank() > max_rank) max_rank = c.get_rank();
            tableau_pile.push_back(c);
        }

        tableau_piles.push_back(tableau_pile);
    }

    // Assign hole card
    assert(doc.HasMember("hole card"));
    const Value& json_h_card = doc["hole card"];
    assert(json_h_card.IsString());
    hole_card = json_h_card.GetString();
}

// Construct an initial game state from a seed
game_state::game_state(int seed, const sol_rules& rules) : rules(rules) {
    vector<card> deck = shuffled_deck(seed, rules.max_rank);

    // Deal to the tableau piles (row-by-row)
    for (vector<card>::size_type i = 0; i < deck.size(); i++) {
        card c = deck[i];

        // If there is a hole card, don't deal the ace of spades
        if (rules.hole && c == "AS") {
            hole_card = "AS";
            continue;
        }

        // For the first row we must create all the tableau pile vectors
        if (i / rules.tableau_pile_count == 0) {
            tableau_piles.push_back(vector<card>());
        }

        // Add the randomly generated card to the tableau piles
        tableau_piles[i % rules.tableau_pile_count].push_back(c);
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
    vector<game_state> next;

    // Searches through each column for a top card that can be moved to the hole
    bool all_tableaux_empty = true;
    for (vector<vector<card>>::size_type i = 0; i < tableau_piles.size(); i++) {
        vector<card> tableau_pile = tableau_piles[i];

        // Ignore empty piles
        if (tableau_pile.empty()) {
            continue;
        } else {
            all_tableaux_empty = false;
        }
        card top_card = tableau_pile.back();

        // If a card at the top of a tableau pile is adjacent to the hole card,
        // move it to the hole and add that state
        if (adjacent(top_card, hole_card)) {
            game_state s = *this;
            s.move_to_hole(i);
            next.push_back(s);
        }
    }

    // If there are no cards left in play, deal solved
    solved = all_tableaux_empty;

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

void game_state::move_to_hole(int tab_idx) {
    hole_card = tableau_piles[tab_idx].back();
    tableau_piles[tab_idx].pop_back();
}

bool game_state::is_solved() const {
    return solved;
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
    stream << foundations[0] << "\t"
           << foundations[1] << "\t"
           << foundations[2] << "\t"
           << foundations[3] << "\n";
};

void game_state::print_tableau_piles(ostream& stream) const {
    bool empty_row = false;
    vector<card>::size_type row_idx = 0;

    while (!empty_row) {
        empty_row = true;

        for (vector<card> tableau_pile : tableau_piles) {
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
    stream << hole_card << "\n";
};

ostream& operator <<(ostream& stream, const game_state& gs) {
    return gs.print(stream);
}