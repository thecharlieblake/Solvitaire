/*
  Solvitaire: a solver for perfect information solitaire games
  Copyright (C) 2018 Charles Blake <thecharlesblake@live.co.uk> and 
  Ian Gent <Ian.Gent@st-andrews.ac.uk>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program (see LICENSE file); if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
//
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>

#include "../../../lib/rapidjson/stringbuffer.h"
#include "../../../lib/rapidjson/document.h"

#include "../../main/input-output/input/json-parsing/deal_parser.h"
#include "../../main/input-output/input/json-parsing/json_helper.h"

using namespace std;
using namespace rapidjson;

void run_deal_parser_test(bool, const char*);

TEST(DealParser, SimpleTableauLetter) {
    run_deal_parser_test(true, R"(
{
  "tableau piles": [
    ["AS"]
  ]
}
)");
}

TEST(DealParser, SimpleTableauNumber) {
    run_deal_parser_test(true, R"(
{
  "tableau piles": [
    ["1S"]
  ]
}
)");
}

TEST(DealParser, IllegalTableauNumber) {
    run_deal_parser_test(false, R"(
{
  "tableau piles": [
    ["14S"]
  ]
}
)");
}

TEST(DealParser, IllegalTableauSuit) {
    run_deal_parser_test(false, R"(
{
  "tableau piles": [
    ["14A"]
  ]
}
)");
}

TEST(DealParser, SimpleTableauCase) {
    run_deal_parser_test(true, R"(
{
  "tableau piles": [
    ["kc"]
  ]
}
)");
}

TEST(DealParser, AdvancedTableau) {
    run_deal_parser_test(true, R"(
{
  "tableau piles": [
    ["5S","13C","4C","3C","2S","2c"],
    ["5D","qD","3S","1d","2D"],
    ["5H","4S","JH","3h","2H"]
  ]
}
)");
}

TEST(DealParser, Hole) {
    run_deal_parser_test(true, R"(
{
  "tableau piles": [
    ["5S","13C","4C"]
  ],
  "hole": "qD"
}
)");
}

TEST(DealParser, Stock) {
    run_deal_parser_test(true, R"(
{
  "tableau piles": [
    ["5S","13C","4C"]
  ],
  "stock": ["1S","JC","7H"]
}
)");
}

TEST(DealParser, Waste) {
    run_deal_parser_test(true, R"(
{
  "tableau piles": [
    ["5S","13C","4C"]
  ],
  "waste": ["1S","JC","7H"]
}
)");
}

TEST(DealParser, IllegalPileType) {
    run_deal_parser_test(false, R"(
{
  "tableau piles": [
    ["5S","13C","4C"]
  ],
  "other": ["1S","JC","7H"]
}
)");
}

void run_deal_parser_test(bool validity, const char* json) {
    Document in_doc;
    in_doc.Parse(json);
    if (in_doc.HasParseError()) {
        throw runtime_error("invalid json");
    }

    Document val_doc;
    val_doc.Parse(deal_parser::deal_schema_json().c_str());
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
