//
// Created by thecharlesblake on 10/21/17.
//

#include "deal_parser.h"
#include "../game/game_state.h"
#include "../util/util.h"

using namespace std;
using namespace rapidjson;

typedef sol_rules::build_order ord;
typedef sol_rules::build_policy pol;

void deal_parser::parse(game_state &gs, const rapidjson::Document& doc) {
    // Construct tableau piles
    if (gs.rules.tableau_pile_count > 0) {
        parse_tableau_piles(gs, doc);
    }
}

void deal_parser::parse_tableau_piles(game_state &gs, const rapidjson::Document& doc) {
    assert(doc.HasMember("tableau piles"));
    const Value& json_tab_piles = doc["tableau piles"];
    assert(json_tab_piles.IsArray());

    for (auto& json_tab : json_tab_piles.GetArray()) {
        pile tableau_pile(true, gs.rules.build_ord, pol::ANY_SUIT, false);

        for (auto& json_card : json_tab.GetArray()) {
            tableau_pile.place(card(json_card.GetString()));
        }

        gs.tableau_piles.push_back(tableau_pile);

        if (gs.tableau_piles.size() != gs.rules.tableau_pile_count) {
            util::json_parse_err("Incorrect number of tableau piles");
        }
    }
}
