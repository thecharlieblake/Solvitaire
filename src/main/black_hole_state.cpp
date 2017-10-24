//
// Created by thecharlesblake on 10/23/17.
//

#include <vector>

#include <rapidjson/document.h>

#include "card.h
#include "game_state.h"
#include "black_hole_state.h"

using namespace std;
using namespace rapidjson;


black_hole_state::black_hole_state(const Document& doc)
        : game_state(doc), solved(false) {
    // Assign hole card
    assert(doc.HasMember("hole card"));
    const Value& json_h_card = doc["hole card"];
    assert(json_h_card.IsArray());
    hole_card = doc["hole card"].GetString();
};

vector<game_state> black_hole_state::get_next_legal_states() const {
    vector<game_state> next;

    bool all_tableaux_empty = true;
    for (int i = 0; i < tableau_piles.size(); i++) {
        vector<card> tableau_pile = tableau_piles[i];
        card top_card = tableau_pile.back();

        if (tableau_pile.empty()) {
            continue;
        } else {
            all_tableaux_empty = false;
        }

        if (adjacent(top_card, hole_card)) {
            black_hole_state s = *this;
            s.move_to_hole(i);
            next.push_back(s);
        }
    }

    // If there are no cards left in play, deal solved
    solved = all_tableaux_empty;

    return next;
}

void black_hole_state::move_to_hole(int tab_idx) {
    hole_card = tableau_piles[tab_idx].back();
    tableau_piles[tab_idx].pop_back();
}

bool black_hole_state::is_solved() const {
    return solved;
}

ostream& black_hole_state::print(ostream& stream) const {
    return game_state::print(stream) << "Hole: " << hole_card << "\n";
}

bool black_hole_state::adjacent(const card& a, const card& b) {
    int x = a.get_rank();
    int y = b.get_rank();

    return x == y + 1
           || x + 1 == y
           || x == 1 && y == MAX_RANK
           || x == MAX_RANK && y == 1;
}