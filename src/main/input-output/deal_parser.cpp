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

    // Construct hole card
    if (gs.rules.hole) {
        parse_hole(gs, doc);
    }
}

void deal_parser::parse_tableau_piles(game_state &gs, const rapidjson::Document& doc) {
    assert(doc.HasMember("tableau piles"));
    const Value& json_tab_piles = doc["tableau piles"];
    assert(json_tab_piles.IsArray());

    if (gs.tableau_piles.size() != gs.rules.tableau_pile_count) {
        util::json_parse_err("Incorrect number of tableau piles");
    }

    for (auto p = std::make_pair(begin(json_tab_piles.GetArray()), begin(gs.tableau_piles));
         p.second != end(gs.tableau_piles);
         ++p.first, ++p.second) {

        for (auto& json_card : p.first->GetArray()) {
            p.second->place(card(json_card.GetString()));
        }
    }
}

void deal_parser::parse_hole(game_state &gs, const rapidjson::Document& doc) {
    assert(doc.HasMember("hole"));
    const Value& json_hole = doc["hole"];
    assert(json_hole.IsString());

    gs.hole.place(card(json_hole.GetString()));
}
