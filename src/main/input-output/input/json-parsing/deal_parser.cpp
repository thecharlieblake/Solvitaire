//
// Created by thecharlesblake on 10/21/17.
//

#include "deal_parser.h"
#include "../../../game/game_state.h"
#include "json_helper.h"

using namespace std;
using namespace rapidjson;

typedef sol_rules::build_policy pol;

void deal_parser::parse(game_state &gs, const rapidjson::Document& doc) {
    // There are two stages to reading in the supplied deal. It is both put
    // through the deal schema validator, and also parsed into
    // the game state object (with some further checking done). The parsing is
    // done first because the errors it detects tend to give clearer error
    // messages than the schema validator

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

    // Construct stock
    if (gs.rules.stock_size > 0) {
        parse_stock(gs, doc);
    }

    // Construct reserve
    if (gs.rules.reserve_size > 0) {
        parse_reserve(gs, doc);
    }

    // If the foundations begin with cards in them, fill them
    if (gs.rules.foundations_init_card) {
        fill_foundations(gs);
    }

    Document d;
    d.Parse(deal_schema_json().c_str());
    assert(!d.HasParseError());
    SchemaDocument sd(d);
    SchemaValidator validator(sd);
    if(!doc.Accept(validator)) {
        throw runtime_error(json_helper::schema_err_str(validator));
    }
}

string deal_parser::deal_schema_json() {
    string schema_json = R"(
{
  "$schema": "http://json-schema.org/draft-04/schema#",
  "description": "JSON Schema representing a solitaire game state to solve",

  "definitions": {
    "card": {"type": "string", "pattern": "^(([0-9]|1[0-3]|a|A|j|J|q|Q|k|K)(c|C|d|D|h|H|s|S))$"},
    "cardarray": {"type": "array", "items": {"$ref": "#/definitions/card"}}
  },

  "type": "object", "properties": {

    "tableau piles": {
      "type": "array", "items": {"$ref": "#/definitions/cardarray"}
    },
    "hole": {"$ref": "#/definitions/card"},
    "stock": {"$ref": "#/definitions/cardarray"},
    "waste": {"$ref": "#/definitions/cardarray"},
    "reserve": {"$ref": "#/definitions/cardarray"}

  }, "additionalProperties": false
}
)";
    return schema_json;
}

void deal_parser::parse_tableau_piles(game_state &gs, const rapidjson::Document& doc) {
    assert(doc.HasMember("tableau piles"));
    const Value& json_tab_piles = doc["tableau piles"];
    assert(json_tab_piles.IsArray());

    if (json_tab_piles.Size() != gs.rules.tableau_pile_count ) {
        json_helper::json_parse_err("Incorrect number of tableau piles");
    }

    for (auto p = std::make_pair(begin(json_tab_piles.GetArray()), begin(gs.original_tableau_piles));
         p.second != end(gs.original_tableau_piles);
         ++p.first, ++p.second) {

        for (auto& json_card : p.first->GetArray()) {
            assert(json_card.IsString());
            gs.place_card(*p.second, card(json_card.GetString()));
        }
    }
}

void deal_parser::parse_hole(game_state &gs, const Document& doc) {
    if (doc.HasMember("hole")) {
        const Value &json_hole = doc["hole"];
        assert(json_hole.IsString());

        gs.place_card(gs.hole, card(json_hole.GetString()));
    }
}

void deal_parser::parse_cells(game_state &gs, const Document& doc) {
    if (doc.HasMember("cells")) {
        const Value &json_cells = doc["cells"];
        assert(json_cells.IsArray());

        const auto json_cell_arr = json_cells.GetArray();

        if (json_cell_arr.Empty()) return;
        else if (json_cell_arr.Size() != gs.rules.cells) {
            json_helper::json_parse_err("Incorrect number of cells");
        }

        for (auto p = std::make_pair(begin(json_cell_arr), begin(gs.original_cells));
             p.second != end(gs.original_cells);
             ++p.first, ++p.second) {

            auto json_card = p.first;
            assert(json_card->IsString());
            gs.place_card(*p.second, card(json_card->GetString()));
        }
    }
}

void deal_parser::parse_stock(game_state &gs, const Document& doc) {
    if (doc.HasMember("stock")) {
        const Value &json_stock = doc["stock"];
        assert(json_stock.IsArray());

        if (json_stock.Size() != gs.rules.stock_size) {
            json_helper::json_parse_err("Incorrect stock size");
        }

        for (const Value& json_card : json_stock.GetArray()) {
            assert(json_card.IsString());
            gs.place_card(gs.stock, card(json_card.GetString()));
        }
    }
}

void deal_parser::parse_reserve(game_state &gs, const Document& doc) {
    assert(doc.HasMember("reserve"));
    const Value& json_reserve_piles = doc["reserve"];
    assert(json_reserve_piles.IsArray());

    const auto& json_card_arr = json_reserve_piles.GetArray();

    // We treat a regular reserve like multiple single-card piles,
    // but a stacked reserve as a single multiple-card pile
    if (json_reserve_piles.Size() != gs.rules.reserve_size) {
        json_helper::json_parse_err("Incorrect reserve size");
    }

    for (game_state::pile_ref i = 0; i < json_card_arr.Size(); i++) {
        assert(json_card_arr[i].IsString());
        game_state::pile_ref pr = gs.original_reserve[0];
        if (!gs.rules.reserve_stacked) pr += i;
        gs.place_card(pr, card(json_card_arr[i].GetString()));
    }
}

void deal_parser::fill_foundations(game_state &gs) {
    auto foundations_count = uint8_t(4 * (gs.rules.two_decks ? 2:1));
    assert(gs.foundations.size() == foundations_count);

    for (uint8_t f_idx = 0; f_idx < foundations_count; f_idx++) {
        gs.place_card(gs.foundations[f_idx], card(f_idx % uint8_t(4), 1));
    }
}
