//
// Created by thecharlesblake on 10/23/17.
//

#include <vector>
#include <ostream>

#include <rapidjson/document.h>

#include "game_state.h"

using namespace std;
using namespace rapidjson;

game_state::game_state(const Document& doc) {
    const Value& json_tab_piles = doc["tableau piles"];
    assert(json_tab_piles.IsArray());

    for (auto& json_tab : json_tab_piles.GetArray()) {
        vector<card> tableau_pile;

        for (auto& json_card : json_tab.GetArray()) {
            tableau_pile.push_back(card(json_card.GetString()));
        }

        tableau_piles.push_back(tableau_pile);
    }
}

ostream& game_state::print(ostream& stream) const {
    bool empty_row = false;
    int row_idx = 0;

    while (!empty_row) {
        empty_row = true;
        for (auto tableau_pile : tableau_piles) {
            if (tableau_pile.size() > row_idx) {
                empty_row = false;
                stream << tableau_pile[row_idx];
            }
            stream << "\t";
        }
        stream << "\n";
        row_idx++;
    }
    return stream;
}

ostream& operator <<(ostream& stream, const game_state& gs) {
    return gs.print(stream);
}