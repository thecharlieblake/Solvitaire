//
// Created by thecharlesblake on 10/21/17.
//

#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include <boost/optional.hpp>
#include <rapidjson/document.h>
#include <rapidjson/schema.h>

#include "deal_parser.h"

using namespace std;
using namespace rapidjson;

deal_parser::deal_parser(const sol_rules &r) : rules(r) {}

game_state deal_parser::parse(const string& filename) const {
    const Document doc = read_file(filename);

    game_state gs(rules);
    /*
    // Construct tableau piles
    assert(doc.HasMember("tableau piles"));
    const Value& json_tab_piles = doc["tableau piles"];
    assert(json_tab_piles.IsArray());

    int max_rank = 1;
    for (auto& json_tab : json_tab_piles.GetArray()) {
        pile tableau_pile(true, rules.build_ord, pol::ANY_SUIT, false);

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
    */
    return gs;
}

Document deal_parser::read_file(const string& filename) {
    // Reads the file into a string
    std::ifstream ifstr(filename);
    std::stringstream buf;
    buf << ifstr.rdbuf();

    if (ifstr.fail()) {
        throw runtime_error("could not read file " + filename);
    } else {
        Document d;

        d.Parse(buf.str().c_str());
        if (d.HasParseError()) {
            throw runtime_error(filename + " not valid json");
        } else {
            return d;
        }
    }
}
