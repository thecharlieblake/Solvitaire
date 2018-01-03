//
// Created by thecharlesblake on 10/21/17.
//

#include "deal_parser.h"
#include "../game/game_state.h"
#include "json_helper.h"

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

    // Construct filled cells
    if (gs.rules.cells) {
        parse_cells(gs, doc);
    }
}

void deal_parser::parse_tableau_piles(game_state &gs, const rapidjson::Document& doc) {
    assert(doc.HasMember("tableau piles"));
    const Value& json_tab_piles = doc["tableau piles"];
    assert(json_tab_piles.IsArray());

    if (gs.tableau_piles.size() != gs.rules.tableau_pile_count) {
        json_helper::json_parse_err("Incorrect number of tableau piles");
    }

    for (auto p = std::make_pair(begin(json_tab_piles.GetArray()), begin(gs.tableau_piles));
         p.second != end(gs.tableau_piles);
         ++p.first, ++p.second) {

        for (auto& json_card : p.first->GetArray()) {
            assert(json_card.IsString());
            gs.piles[*p.second].place(card(json_card.GetString()));
        }
    }
}

void deal_parser::parse_hole(game_state &gs, const Document& doc) {
    if (doc.HasMember("hole")) {
        const Value &json_hole = doc["hole"];
        assert(json_hole.IsString());

        gs.piles[gs.hole].place(card(json_hole.GetString()));
    }
}

void deal_parser::parse_cells(game_state &gs, const Document& doc) {
    if (doc.HasMember("cells")) {
        const Value &json_cells = doc["cells"];
        assert(json_cells.IsArray());

        const auto json_cell_arr = json_cells.GetArray();

        if (json_cell_arr.Empty()) return;
        else if (gs.rules.cells != json_cell_arr.Size()) {
            json_helper::json_parse_err("Incorrect number of cells");
        }

        for (auto p = std::make_pair(begin(json_cell_arr), begin(gs.cells));
             p.second != end(gs.cells);
             ++p.first, ++p.second) {

            auto json_card = p.first;
            assert(json_card->IsString());
            gs.piles[*p.second].place(card(json_card->GetString()));
        }
    }
}
