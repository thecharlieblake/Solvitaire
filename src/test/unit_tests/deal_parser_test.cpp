//
// Created by thecharlesblake on 1/11/18.
//

#include <gtest/gtest.h>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>

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
