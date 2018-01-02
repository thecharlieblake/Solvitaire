//
// Created by thecharlesblake on 10/28/17.
//

#include <string>
#include <vector>
#include <algorithm>

#include "rapidjson/error/en.h"

#include "sol_rules.h"
#include "../util/util.h"
#include "../input-output/sol_preset_types.h"

using namespace std;
using namespace boost;
using namespace rapidjson;

////////////
// Static //
////////////

typedef sol_rules::build_policy pol;

bool sol_rules::is_suit(pol bp) {
    switch (bp) {
        case pol::CLUBS    :
        case pol::DIAMONDS :
        case pol::HEARTS   :
        case pol::SPADES   : return true;
        default            : return false;
    }
}
card::suit_t sol_rules::suit_val(pol bp) {
    switch (bp) {
        case pol::CLUBS    : return card::suit_t::Clubs;
        case pol::DIAMONDS : return card::suit_t::Diamonds;
        case pol::HEARTS   : return card::suit_t::Hearts;
        case pol::SPADES   : return card::suit_t::Spades;
        default: assert(false); return card::suit_t::Spades;
    }
}

const sol_rules sol_rules::from_file(const string rules_file) {
    sol_rules sr = get_default();
    Document d = util::get_file_json(rules_file);

    modify_sol_rules(sr, d);

    return sr;
}

const sol_rules sol_rules::from_preset(const string sol_type) {
    sol_rules sr = get_default();

    const string preset_json = sol_preset_types::get(sol_type);

    Document d;
    d.Parse(preset_json.c_str());
    assert(!d.HasParseError());

    modify_sol_rules(sr, d);
    return sr;
}

sol_rules sol_rules::get_default() {
    sol_rules sr;
    const string default_json = sol_preset_types::get("default");

    Document d;
    d.Parse(default_json.c_str());
    assert(!d.HasParseError());

    modify_sol_rules(sr, d);
    return sr;
}

void sol_rules::modify_sol_rules(sol_rules& sr, Document& d) {

    if (!d.IsObject()) {
        util::json_parse_err("JSON doc must be object");
    }

    if (d.HasMember("tableau piles")) {
        if (d["tableau piles"].IsObject()) {

            if (d["tableau piles"].HasMember("count")) {
                if (d["tableau piles"]["count"].IsInt()) {
                    sr.tableau_pile_count = static_cast<uint8_t>(d["tableau piles"]["count"].GetInt());
                } else {
                    util::json_parse_err("[tableau piles][count] must be an integer");
                }
            }

            if (d["tableau piles"].HasMember("build order")) {
                if (d["tableau piles"]["build order"].IsString()) {
                    string bo_str = d["tableau piles"]["build order"].GetString();

                    if (bo_str == "no-build") {
                        sr.build_ord = build_order::NO_BUILD;
                    } else if (bo_str == "descending") {
                        sr.build_ord = build_order::DESCENDING;
                    } else {
                        util::json_parse_err("[tableau piles][build order] is invalid");
                    }

                } else {
                    util::json_parse_err("[tableau piles][build order] must be an string");
                }
            }

            if (d["tableau piles"].HasMember("build policy")) {
                if (d["tableau piles"]["build policy"].IsString()) {
                    string bp_str = d["tableau piles"]["build policy"].GetString();

                    if (bp_str == "any-suit") {
                        sr.build_pol = build_policy::ANY_SUIT;
                    } else if (bp_str == "red-black") {
                        sr.build_pol = build_policy::RED_BLACK;
                    } else {
                        util::json_parse_err("[tableau piles][build policy] is invalid");
                    }

                } else {
                    util::json_parse_err("[tableau piles][build policy] must be an string");
                }
            }

            if (d["tableau piles"].HasMember("spaces policy")) {
                if (d["tableau piles"]["spaces policy"].IsString()) {
                    string sp_str = d["tableau piles"]["spaces policy"].GetString();

                    if (sp_str == "any") {
                        sr.spaces_pol = spaces_policy::ANY;
                    } else if (sp_str == "no-build") {
                        sr.spaces_pol = spaces_policy::NO_BUILD;
                    } else {
                        util::json_parse_err("[tableau piles][spaces policy] is invalid");
                    }

                } else {
                    util::json_parse_err("[tableau piles][spaces policy] must be an string");
                }
            }

        } else {
            util::json_parse_err("[tableau piles] must be an object");
        }
    }

    if (d.HasMember("max rank")) {
        if (d["max rank"].IsInt()) {
            sr.max_rank = static_cast<card::rank_t>(d["max rank"].GetInt());
        } else {
            util::json_parse_err("[max rank] must be an integer");
        }
    }

    if (d.HasMember("hole")) {
        if (d["hole"].IsBool()) {
            sr.hole = d["hole"].GetBool();
        } else {
            util::json_parse_err("[hole] must be a boolean");
        }
    }

    if (d.HasMember("foundations")) {
        if (d["foundations"].IsBool()) {
            sr.foundations = d["foundations"].GetBool();
        } else {
            util::json_parse_err("[foundations] must be a boolean");
        }
    }

    if (d.HasMember("cells")) {
        if (d["cells"].IsInt()) {
            sr.cells = static_cast<uint8_t>(d["cells"].GetInt());
        } else {
            util::json_parse_err("[cells] must be an integer");
        }
    }

    if (d.HasMember("reserve size")) {
        if (d["reserve size"].IsInt()) {
            sr.reserve_size = static_cast<uint8_t>(d["reserve size"].GetInt());
        } else {
            util::json_parse_err("[reserve size] must be an integer");
        }
    }

    if (d.HasMember("stock size")) {
        if (d["stock size"].IsInt()) {
            sr.stock_size = static_cast<uint8_t>(d["stock size"].GetInt());
        } else {
            util::json_parse_err("[stock size] must be an integer");
        }
    }
}
