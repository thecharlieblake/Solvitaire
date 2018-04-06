//
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>

#include "../../../lib/rapidjson/stringbuffer.h"
#include "../../../lib/rapidjson/document.h"

#include "../../main/input-output/input/json-parsing/rules_parser.h"
#include "../../main/input-output/input/json-parsing/json_helper.h"

using namespace std;
using namespace rapidjson;

void run_rules_parser_test(bool, const char*);

// invalid int, build pol, bool, other

TEST(RulesParser, DefaultGameRules) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 8,
    "build policy": "any-suit",
    "spaces policy": "any",
    "diagonal deal": false,
    "move built group": false,
    "move built group policy": "same-as-build"
  },
  "max rank": 13,
  "two decks": false,
  "hole": false,
  "foundations": true,
  "foundations initial card": false,
  "foundations removable": false,
  "foundations complete piles": false,
  "cells": 0,
  "stock size": 0,
  "stock deal type": "waste",
  "reserve size": 0,
  "reserve stacked": false
}
)");
}

TEST(RulesParser, InvalidType) {
    run_rules_parser_test(false, R"(
{
  "tableau piles": {
    "count": "13"
  }
}
)");
}

TEST(RulesParser, InvalidNumber) {
    run_rules_parser_test(false, R"(
{
  "tableau piles": {
    "count": 13
  },
  "max rank": 20
}
)");
}

TEST(RulesParser, InvalidEnum) {
    run_rules_parser_test(false, R"(
{
  "tableau piles": {
    "count": 13,
    "build policy": "red-black-green"
  }
}
)");
}

TEST(RulesParser, InvalidRuleName) {
    run_rules_parser_test(false, R"(
{
  "tableau piles": {
    "count number": 13
  }
}
)");
}

TEST(RulesParser, SpanishPatience) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 13
  },
  "foundations removable": true
}
)");
}

TEST(RulesParser, SimpleSpanishPatience) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 3
  },
  "max rank": 2,
  "foundations removable": true
}
)");
}

TEST(RulesParser, FreeCell) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "build policy": "red-black"
  },
  "cells": 4
}
)");
}

TEST(RulesParser, SimpleFreeCell) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 4,
    "build policy": "red-black"
  },
  "max rank": 4,
  "cells": 1
}
)");
}

TEST(RulesParser, BakersDozen) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 13,
    "spaces policy": "no-build"
  },
  "foundations removable": true
}
)");
}

TEST(RulesParser, SimpleBakersDozen) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 3,
    "spaces policy": "no-build"
  },
  "max rank": 3,
  "foundations removable": true
}
)");
}

TEST(RulesParser, BlackHole) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 17,
    "build policy": "no-build"
  },
  "hole": true,
  "foundations": false
}
)");
}

TEST(RulesParser, SimpleBlackHole) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 4,
    "build policy": "no-build"
  },
  "max rank": 5,
  "hole": true,
  "foundations": false
}
)");
}

TEST(RulesParser, FortunesFavor) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 12,
    "build policy": "same-suit"
  },
  "foundations initial card": true,
  "stock size": 36
}
)");
}

TEST(RulesParser, SimpleFortunesFavor) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 3,
    "build policy": "same-suit"
  },
  "max rank": 4,
  "foundations initial card": true,
  "stock size": 3
}
)");
}

TEST(RulesParser, FlowerGarden) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 6
  },
  "reserve size": 16,
  "foundations removable": true
}
)");
}

TEST(RulesParser, SimpleFlowerGarden) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 3
  },
  "max rank": 3,
  "reserve size": 3,
  "foundations removable": true
}
)");
}

TEST(RulesParser, Canfield) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 4,
    "build policy": "red-black"
  },
  "stock size": 34,
  "reserve size": 14,
  "reserve stacked": true
}
)");
}

TEST(RulesParser, SimpleCanfield) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 3,
    "build policy": "red-black"
  },
  "max rank": 3,
  "stock size": 1,
  "reserve size": 2,
  "reserve stacked": true
}
)");
}

TEST(RulesParser, Somerset) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 10,
    "build policy": "red-black",
    "diagonal deal": true
  },
  "foundations removable": true
}
)");
}

TEST(RulesParser, SimpleSomerset) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 4,
    "build policy": "red-black",
    "diagonal deal": true
  },
  "max rank": 2,
  "foundations removable": true
}
)");
}

TEST(RulesParser, AlphaStar) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 12,
    "build policy": "same-suit",
    "move built group": true
  },
  "foundations initial card": true
}
)");
}

TEST(RulesParser, SimpleAlphaStar) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 3,
    "build policy": "same-suit",
    "move built group": true
  },
  "max rank": 5,
  "foundations initial card": true
}
)");
}

TEST(RulesParser, Spider) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 10,
    "build policy": "any-suit",
    "move built group": true,
    "move built group policy": "same-suit"
  },
  "foundations complete piles": true,
  "two decks": true,
  "stock size": 50,
  "stock deal type": "tableau piles"
}
)");
}

TEST(RulesParser, SimpleSpider) {
    run_rules_parser_test(true, R"(
{
  "tableau piles": {
    "count": 4,
    "build policy": "any-suit",
    "move built group": true,
    "move built group policy": "same-suit"
  },
  "foundations complete piles": true,
  "two decks": true,
  "max rank": 3,
  "stock size": 12,
  "stock deal type": "tableau piles"
}
)");
}

void run_rules_parser_test(bool validity, const char* json) {
    Document in_doc;
    in_doc.Parse(json);
    if (in_doc.HasParseError()) {
        throw runtime_error("invalid json");
    }

    Document val_doc;
    val_doc.Parse(rules_parser::rules_schema_json().c_str());
    SchemaDocument sd(val_doc);
    SchemaValidator validator(sd);
    bool acc = in_doc.Accept(validator);
    if (acc) {
        ASSERT_TRUE(validity == acc) << "JSON accepted when it should not have "
                                        "been:\n" << json;
    } else {
        ASSERT_TRUE(validity == acc) << json_helper::schema_err_str(validator)
                                     << "\n" << json;
    }
}
