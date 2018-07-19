//
// Created by thecharlesblake on 1/3/18.
//

#include <vector>
#include <algorithm>

#include "rules_parser.h"
#include "json_helper.h"
#include "../sol_preset_types.h"
#include "../../../game/card.h"
#include "../../../game/sol_rules.h"

using namespace std;
using namespace rapidjson;

////////////
// Static //
////////////

typedef sol_rules::build_policy pol;
typedef sol_rules::spaces_policy s_pol;
typedef sol_rules::accordion_policy acc_pol;
typedef sol_rules::stock_deal_type sdt;
typedef sol_rules::face_up_policy fu;
typedef sol_rules::direction dir;
typedef sol_rules::built_group_type bgt;
typedef sol_rules::foundations_init_type fit;

const sol_rules rules_parser::from_file(const string rules_file) {
    sol_rules sr = get_default();
    Document d = json_helper::get_file_json(rules_file);

    modify_sol_rules(sr, d);

    return sr;
}

const sol_rules rules_parser::from_preset(const string sol_type) {
    sol_rules sr = get_default();

    const string preset_json = sol_preset_types::get(sol_type);

    Document d;
    d.Parse(preset_json.c_str());
    assert(!d.HasParseError());

    modify_sol_rules(sr, d);
    return sr;
}

sol_rules rules_parser::get_default() {
    sol_rules sr;
    const string default_json = sol_preset_types::get("default");

    Document d;
    d.Parse(default_json.c_str());
    assert(!d.HasParseError());

    modify_sol_rules(sr, d);
    return sr;
}

void rules_parser::modify_sol_rules(sol_rules& sr, Document& d) {
    // There are two stages to reading in a rules document. It is both put
    // through the rule schema validator, and also parsed into
    // the game rules object (with some further checking done). The parsing is
    // done first because the errors it detects tend to give clearer error
    // messages than the schema validator

    if (!d.IsObject()) {
        json_helper::json_parse_err("JSON doc must be object");
    }

    if (d.HasMember("tableau piles")) {
        if (d["tableau piles"].IsObject()) {

            if (d["tableau piles"].HasMember("count")) {
                if (d["tableau piles"]["count"].IsInt()) {
                    sr.tableau_pile_count = static_cast<uint8_t>(d["tableau piles"]["count"].GetInt());
                    if (sr.tableau_pile_count > 52) {
                        json_helper::json_parse_warning("[tableau piles][count] may be too high");
                    }
                } else {
                    json_helper::json_parse_err("[tableau piles][count] must be an integer");
                }
            }

            if (d["tableau piles"].HasMember("build policy")) {
                if (d["tableau piles"]["build policy"].IsString()) {
                    string bp_str = d["tableau piles"]["build policy"].GetString();

                    if (bp_str == "any-suit") {
                        sr.build_pol = pol::ANY_SUIT;
                    } else if (bp_str == "red-black") {
                        sr.build_pol = pol::RED_BLACK;
                    } else if (bp_str == "same-suit") {
                        sr.build_pol = pol::SAME_SUIT;
                    } else if (bp_str == "no-build") {
                        sr.build_pol = pol::NO_BUILD;
                    } else {
                        json_helper::json_parse_err("[tableau piles][build policy] is invalid");
                    }

                } else {
                    json_helper::json_parse_err("[tableau piles][build policy] must be an string");
                }
            }

            if (d["tableau piles"].HasMember("spaces policy")) {
                if (d["tableau piles"]["spaces policy"].IsString()) {
                    string sp_str = d["tableau piles"]["spaces policy"].GetString();

                    if (sp_str == "any") {
                        sr.spaces_pol = s_pol::ANY;
                    } else if (sp_str == "no-build") {
                        sr.spaces_pol = s_pol::NO_BUILD;
                    } else if (sp_str == "kings") {
                        sr.spaces_pol = s_pol::KINGS;
                    } else {
                        json_helper::json_parse_err("[tableau piles][spaces policy] is invalid");
                    }

                } else {
                    json_helper::json_parse_err("[tableau piles][spaces policy] must be a string");
                }
            }

            if (d["tableau piles"].HasMember("move built group")) {
                if (d["tableau piles"]["move built group"].IsString()) {
                    string mbg_str = d["tableau piles"]["move built group"].GetString();

                    if (mbg_str == "yes") {
                        sr.move_built_group = bgt::YES;
                    } else if (mbg_str == "no") {
                        sr.move_built_group = bgt::NO;
                    } else if (mbg_str == "whole-pile") {
                        sr.move_built_group = bgt::WHOLE_PILE;
                    } else {
                        json_helper::json_parse_err("[tableau piles][move built group] is invalid");
                    }
                } else {
                    json_helper::json_parse_err("[tableau piles][move built group] must be a string");
                }
            }

            string mbgp_str;
            if (d["tableau piles"].HasMember("move built group policy")) {
                if (d["tableau piles"]["move built group policy"].IsString()) {
                    mbgp_str = d["tableau piles"]["move built group policy"].GetString();
                } else {
                    json_helper::json_parse_err("[tableau piles][move built group policy] must be a string");
                }
            } else {
                mbgp_str = "same-as-build";
            }
            if (mbgp_str == "same-as-build") {
                sr.built_group_pol = sr.build_pol;
            } else if (mbgp_str == "any-suit") {
                sr.built_group_pol = pol::ANY_SUIT;
            } else if (mbgp_str == "red-black") {
                sr.built_group_pol = pol::RED_BLACK;
            } else if (mbgp_str == "same-suit") {
                sr.built_group_pol = pol::SAME_SUIT;
            } else if (mbgp_str == "no-build") {
                sr.built_group_pol = pol::NO_BUILD;
            } else {
                json_helper::json_parse_err("[tableau piles][move built group policy] is invalid");
            }

            if (d["tableau piles"].HasMember("diagonal deal")) {
                if (d["tableau piles"]["diagonal deal"].IsBool()) {
                    sr.diagonal_deal = d["tableau piles"]["diagonal deal"].GetBool();
                } else {
                    json_helper::json_parse_err("[tableau piles][diagonal deal] must be a boolean");
                }
            }

            string face_up_str;
            if (d["tableau piles"].HasMember("face up cards")) {
                if (d["tableau piles"]["face up cards"].IsString()) {
                    face_up_str = d["tableau piles"]["face up cards"].GetString();

                    if (face_up_str == "all") {
                        sr.face_up = fu::ALL;
                    } else if (face_up_str == "top") {
                        sr.face_up = fu::TOP_CARDS;
                    } else {
                        string err = "[tableau piles][face up cards] is invalid: " + face_up_str;
                        json_helper::json_parse_err(err);
                    }

                } else {
                    json_helper::json_parse_err("[tableau piles][face up cards] must be a string");
                }
            }

        } else {
            json_helper::json_parse_err("[tableau piles] must be an object");
        }
    }

    if (d.HasMember("foundations")) {
        if (d["foundations"].IsObject()) {
            if (d["foundations"].HasMember("present")) {
                if (d["foundations"]["present"].IsBool()) {
                    sr.foundations_present = d["foundations"]["present"].GetBool();
                } else {
                    json_helper::json_parse_err("[foundations][present] must be a boolean");
                }
            }

            if (d["foundations"].HasMember("initial cards")) {
                if (d["foundations"]["initial cards"].IsString()) {
                    string ic_str = d["foundations"]["initial cards"].GetString();

                    if (ic_str == "none") {
                        sr.foundations_init_cards = fit::NONE;
                    } else if (ic_str == "one") {
                        sr.foundations_init_cards = fit::ONE;
                    } else if (ic_str == "all") {
                        sr.foundations_init_cards = fit::ALL;
                    } else {
                        string err = "[foundations][initial cards] is invalid: " + ic_str;
                        json_helper::json_parse_err(err);
                    }
                } else {
                    json_helper::json_parse_err("[foundations][initial cards] must be a string");
                }
            }

            if (d["foundations"].HasMember("removable")) {
                if (d["foundations"]["removable"].IsBool()) {
                    sr.foundations_removable = d["foundations"]["removable"].GetBool();
                } else {
                    json_helper::json_parse_err("[foundations][removable] must be a boolean");
                }
            }

            if (d["foundations"].HasMember("complete piles")) {
                if (d["foundations"]["complete piles"].IsBool()) {
                    sr.foundations_only_comp_piles = d["foundations"]["complete piles"].GetBool();
                } else {
                    json_helper::json_parse_err("[foundations][complete piles] must be a boolean");
                }
            }

        } else {
            json_helper::json_parse_err("[foundations] must be an object");
        }
    }

    if (d.HasMember("hole")) {
        if (d["hole"].IsBool()) {
            sr.hole = d["hole"].GetBool();
        } else {
            json_helper::json_parse_err("[hole] must be a boolean");
        }
    }

    if (d.HasMember("cells")) {
        if (d["cells"].IsInt()) {
            sr.cells = static_cast<uint8_t>(d["cells"].GetInt());
        } else {
            json_helper::json_parse_err("[cells] must be an integer");
        }
    }

    if (d.HasMember("stock")) {
        if (d["stock"].IsObject()) {
            if (d["stock"].HasMember("size")) {
                if (d["stock"]["size"].IsInt()) {
                    sr.stock_size = static_cast<uint8_t>(d["stock"]["size"].GetInt());
                } else {
                    json_helper::json_parse_err("[stock][size] must be an integer");
                }
            }

            if (d["stock"].HasMember("deal type")) {
                if (d["stock"]["deal type"].IsString()) {
                    if(string(d["stock"]["deal type"].GetString()) == "waste") {
                        sr.stock_deal_t = sdt::WASTE;
                    } else {
                        sr.stock_deal_t = sdt::TABLEAU_PILES;
                    }
                } else {
                    json_helper::json_parse_err("[stock][deal type] must be a string");
                }
            }

            if (d["stock"].HasMember("deal count")) {
                if (d["stock"]["deal count"].IsInt()) {
                    sr.stock_deal_count = static_cast<uint8_t>(d["stock"]["deal count"].GetInt());
                } else {
                    json_helper::json_parse_err("[stock][deal count] must be an integer");
                }
            }

            if (d["stock"].HasMember("redeal")) {
                if (d["stock"]["redeal"].IsBool()) {
                    sr.stock_redeal = static_cast<uint8_t>(d["stock"]["redeal"].GetBool());
                } else {
                    json_helper::json_parse_err("[stock][redeal] must be a boolean");
                }
            }
        } else {
            json_helper::json_parse_err("[stock] must be an object");
        }
    }

    if (d.HasMember("reserve")) {
        if (d["reserve"].IsObject()) {
            if (d["reserve"].HasMember("size")) {
                if (d["reserve"]["size"].IsInt()) {
                    sr.reserve_size = static_cast<uint8_t>(d["reserve"]["size"].GetInt());
                } else {
                    json_helper::json_parse_err("[size] must be an integer");
                }
            }

            if (d["reserve"].HasMember("stacked")) {
                if (d["reserve"]["stacked"].IsBool()) {
                    sr.reserve_stacked = d["reserve"]["stacked"].GetBool();
                } else {
                    json_helper::json_parse_err("[stacked] must be a boolean");
                }
            }
        } else {
            json_helper::json_parse_err("[reserve] must be an object");
        }
    }

    if (d.HasMember("accordion")) {
        if (d["accordion"].IsObject()) {
            if (d["accordion"].HasMember("size")) {
                if (d["accordion"]["size"].IsInt()) {
                    sr.accordion_size = static_cast<uint8_t>(d["accordion"]["size"].GetInt());
                } else {
                    json_helper::json_parse_err("[size] must be an integer");
                }
            }

            if (d["accordion"].HasMember("moves")) {
                if (d["accordion"]["moves"].IsArray()) {
                    for (auto& json_move : d["accordion"]["moves"].GetArray()) {
                        string move_str = json_move.GetString();

                        sol_rules::direction move_dir;
                        if (move_str[0] == 'L') {
                            move_dir = sol_rules::direction::LEFT;
                        } else if (move_str[0] == 'R') {
                            move_dir = sol_rules::direction::RIGHT;
                        } else {
                            json_helper::json_parse_err(R"([accordion][moves] must begin 'L' or 'R')");
                        }

                        auto count_str = move_str.substr(1, move_str.size() - 1);
                        auto count = static_cast<uint8_t>(stoi(count_str));

                        sr.accordion_moves.emplace_back(move_dir, count);
                    }
                } else {
                    json_helper::json_parse_err("[accordion][moves] must be an array");
                }
            }

            if (d["accordion"].HasMember("build policies")) {
                if (d["accordion"]["build policies"].IsArray()) {
                    for (auto& json_policy : d["accordion"]["build policies"].GetArray()) {
                        string policy_str = json_policy.GetString();

                        if (policy_str == "any-suit") {
                            sr.accordion_pol.emplace_back(acc_pol::ANY_SUIT);
                        } else if (policy_str == "red-black") {
                            sr.accordion_pol.emplace_back(acc_pol::RED_BLACK);
                        } else if (policy_str == "same-suit") {
                            sr.accordion_pol.emplace_back(acc_pol::SAME_SUIT);
                        } else if (policy_str == "same-rank") {
                            sr.accordion_pol.emplace_back(acc_pol::SAME_RANK);
                        } else {
                            json_helper::json_parse_err("[accordion][build policies] is invalid");
                        }
                    }
                } else {
                    json_helper::json_parse_err("[accordion][build policies] must be an array");
                }
            }
        } else {
            json_helper::json_parse_err("[accordion] must be an object");
        }
    }

    if (d.HasMember("sequences")) {
        if (d["sequences"].IsObject()) {
            if (d["sequences"].HasMember("count")) {
                if (d["sequences"]["count"].IsInt()) {
                    sr.sequence_count = static_cast<uint8_t>(d["sequences"]["count"].GetInt());
                } else {
                    json_helper::json_parse_err("[count] must be an integer");
                }
            }

            if (d["sequences"].HasMember("build policy")) {
                if (d["sequences"]["build policy"].IsString()) {
                    string bp_str = d["sequences"]["build policy"].GetString();

                    if (bp_str == "any-suit") {
                        sr.sequence_build_pol = pol::ANY_SUIT;
                    } else if (bp_str == "red-black") {
                        sr.sequence_build_pol = pol::RED_BLACK;
                    } else if (bp_str == "same-suit") {
                        sr.sequence_build_pol = pol::SAME_SUIT;
                    } else {
                        json_helper::json_parse_err("[sequences][build policy] is invalid");
                    }
                } else {
                    json_helper::json_parse_err("[sequences][build policy] must be an string");
                }
            }

            if (d["sequences"].HasMember("direction")) {
                if (d["sequences"]["direction"].IsString()) {
                    string dir_str = d["sequences"]["direction"].GetString();

                    if (dir_str == "L") {
                        sr.sequence_direction = dir::LEFT;
                    } else if (dir_str == "R") {
                        sr.sequence_direction = dir::RIGHT;
                    } else if (dir_str == "LR") {
                        sr.sequence_direction = dir::BOTH;
                    } else {
                        json_helper::json_parse_err("[sequences][direction] is invalid");
                    }
                } else {
                    json_helper::json_parse_err("[sequences][direction] must be an string");
                }
            }
        } else {
            json_helper::json_parse_err("[sequences] must be an object");
        }
    }

    if (d.HasMember("max rank")) {
        if (d["max rank"].IsInt()) {
            sr.max_rank = static_cast<card::rank_t>(d["max rank"].GetInt());
            if (sr.max_rank > 13) {
                json_helper::json_parse_err("[max rank] must be a valid rank");
            }
        } else {
            json_helper::json_parse_err("[max rank] must be an integer");
        }
    }

    if (d.HasMember("two decks")) {
        if (d["two decks"].IsBool()) {
            sr.two_decks = d["two decks"].GetBool();
        } else {
            json_helper::json_parse_err("[two decks] must be a boolean");
        }
    }

    int solution_types = 0;
    if (sr.hole) solution_types++;
    if (sr.foundations_present) solution_types++;
    if (sr.sequence_count > 0) solution_types++;
    if (sr.accordion_size > 0) solution_types++;
    if (solution_types != 1) {
        json_helper::json_parse_err("one and only one of [hole], [foundations], [accordion][size] > 0 and [sequences][count] > 0 "
                                    "must be true");
    }

    rules_schema_json(); // Throws an error if it fails
    Document vd;
    vd.Parse(rules_schema_json().c_str());
    assert(!vd.HasParseError());
    SchemaDocument sd(vd);
    SchemaValidator validator(sd);
    if(!d.Accept(validator)) {
        throw runtime_error(json_helper::schema_err_str(validator));
    }
}

string rules_parser::rules_schema_json() {
    string schema_json = R"(
{
  "$schema": "http://json-schema.org/draft-04/schema#",
  "description": "JSON Schema representing a generic solitaire game",
  "definitions": {
    "rank": {"type": "string", "pattern": "^(([0-9]|1[0-3]|a|A|j|J|q|Q|k|K))$"},
  },
  "type": "object",
  "properties": {
    "tableau piles": {
      "type": "object",
      "properties": {
        "count": {
          "type": "integer",
          "minimum": 0
        },
        "build policy": {
          "type": "string",
          "enum": [
            "any-suit",
            "red-black",
            "same-suit",
            "no-build"
          ]
        },
        "spaces policy": {
          "type": "string",
          "enum": [
            "any",
            "no-build",
            "kings",
            "auto-reserve"
          ]
        },
        "diagonal deal": {
          "type": "boolean"
        },
        "wraps": {
          "type": "boolean"
        },
        "move built group": {
          "type": "string",
          "enum": [
            "yes",
            "no",
            "whole-pile"
          ]
        },
        "move built group policy": {
          "type": "string",
          "enum": [
            "same-as-build",
            "any-suit",
            "red-black",
            "same-suit",
            "no-build"
          ]
        },
        "face up cards": {
          "type": "string",
          "enum": [
            "all",
            "top"
          ]
        }
      },
      "additionalProperties": false
    },
    "foundations": {
      "type": "object",
      "properties": {
        "present": {
          "type": "boolean"
        },
        "initial cards": {
          "type": "string",
          "enum": [
            "none",
            "one",
            "all"
          ]
        },
        "base card": {
          "type": "string",
          "oneOf":[
          	{"$ref": "#/definitions/rank"},
          	{"type": "string","enum": ["random"]}
          ]
        },
        "removable": {
          "type": "boolean"
        },
        "only complete pile moves": {
          "type": "boolean"
        }
      },
      "additionalProperties": false
    },
    "hole": {
      "type": "boolean"
    },
    "cells": {
      "type": "integer",
      "minimum": 0
    },
    "stock": {
      "type": "object",
      "properties": {
        "size": {
          "type": "integer",
          "minimum": 0
        },
        "deal type": {
          "type": "string",
          "enum": [
            "waste",
            "tableau piles"
          ]
        },
        "deal count": {
          "type": "integer",
          "minimum": 1
        },
        "redeal": {
          "type": "boolean"
        }
      },
      "additionalProperties": false
    },
    "reserve": {
      "type": "object",
      "properties": {
        "size": {
          "type": "integer",
          "minimum": 0
        },
        "stacked": {
          "type": "boolean"
        }
      },
      "additionalProperties": false
    },
    "accordion": {
      "type": "object",
      "properties": {
        "size": {
          "type": "integer",
          "minimum": 0
        },
        "moves": {
          "items": {
            "type": "string",
            "pattern": "^((L|R)([1-9]|[1-4][0-9]|5[0-2]))$"
          }
        },
        "build policies": {
          "type": "array",
          "items": {
            "type": "string",
            "enum": [
              "same-suit",
              "red-black",
              "any-suit",
              "same-rank"
            ]
          }
        }
      },
      "additionalProperties": false
    },
    "sequences": {
      "type": "object",
      "properties": {
        "count": {
          "type": "integer",
          "minimum": 0
        },
        "direction": {
          "type": "string",
          "enum": [
            "L",
            "R",
            "LR"
          ]
        },
        "build policy": {
          "type": "string",
          "enum": [
            "any-suit",
            "red-black",
            "same-suit"
          ]
        }
      },
      "additionalProperties": false
    },
    "max rank": {
      "type": "integer",
      "minimum": 1,
      "maximum": 13
    },
    "two decks": {
      "type": "boolean"
    }
  },
  "additionalProperties": false
}
)";
    return schema_json;
}
