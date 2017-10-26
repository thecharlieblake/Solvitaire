//
// Created by thecharlesblake on 10/24/17.
//

#include <vector>
#include <ostream>

#include <rapidjson/document.h>

#include "game_state.h"

using namespace rapidjson;
using namespace std;

// Construct an initial game state from a JSON doc
game_state::game_state(const Document& doc) : max_rank(1) {
    // Construct tableau piles
    assert(doc.HasMember("tableau piles"));
    const Value& json_tab_piles = doc["tableau piles"];
    assert(json_tab_piles.IsArray());

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
           || (x == 1 && y == max_rank)
           || (x == max_rank && y == 1);
}

void game_state::move_to_hole(int tab_idx) {
    hole_card = tableau_piles[tab_idx].back();
    tableau_piles[tab_idx].pop_back();
}

bool game_state::is_solved() const {
    return solved;
}

ostream& game_state::print(ostream& stream) const {
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

        if (!empty_row) stream << "|";
        if (row_idx == 0) {
            stream << " Hole card: " << hole_card;
        }

        stream << "\n";
        row_idx++;
    }

    return stream;
}

ostream& operator <<(ostream& stream, const game_state& gs) {
    return gs.print(stream);
}