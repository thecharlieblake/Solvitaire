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
#include <rapidjson/stringbuffer.h>

#include "deal_parser.h"

using namespace std;
using namespace rapidjson;
using namespace boost;

void deal_parser::parse(Document& doc, const std::string filename) {
    const optional<string> in_json = read_file(filename);

    if (!in_json) {
        throw runtime_error("could not read file " + filename);
    }

    Document sd;
    gen_schema_doc(sd);
    SchemaDocument schema(sd);
    SchemaValidator validator(schema);

    if(!to_json(doc, *in_json)) {
        throw runtime_error(filename + " not valid json");
    }

    if (!doc.Accept(validator)) {
        throw runtime_error(schema_err_str(validator));
    }
}

const optional<string> deal_parser::read_file(const string filename) {
    // Reads the file into a string
    std::ifstream ifstr(filename);
    std::stringstream buf;
    buf << ifstr.rdbuf();

    if (ifstr.fail()) {
        return none;
    }

    return buf.str();
}

void deal_parser::gen_schema_doc(Document& d) {
    const char* schema_json = "{\"$schema\":\"http://json-schema.org/draft-04/sc"
            "hema#\",\"definitions\":{\"card\":{\"type\":\"string\",\"pattern"
            "\":\"^(?i)([A|[1-9]|1[0-3]|J|Q|K])([C|D|H|S])$\"}},\"type\":\""
            "object\",\"properties\":{\"tableau piles\":{\"type\":\"array\""
            ",\"items\":{\"type\":\"array\",\"items\":{\"$ref\":\"#/definit"
            "ions/card\"}}},\"hole card\":{\"$ref\":\"#/definitions/card\"}}"
            ",\"additionalProperties\":false,\"anyOf\":[{\"required\":[\"tableau"
            " piles\"]}]}";

    to_json(d, schema_json);
}

bool deal_parser::to_json(rapidjson::Document& d, const char* in) {
    d.Parse(in);
    return !d.HasParseError();
}

bool deal_parser::to_json(rapidjson::Document& d, const string in) {
    return to_json(d, in.c_str());
}

const string deal_parser::schema_err_str(const SchemaValidator& validator) {
    // Input JSON is invalid according to the schema
    // Output diagnostic information
    string ret = "input JSON failed to match the required schema.\n";

    StringBuffer sb;
    validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
    ret += "Invalid schema keyword: ";
    ret += validator.GetInvalidSchemaKeyword();
    ret += "\n";

    sb.Clear();
    validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);

    ret += "Invalid document: ";
    ret += sb.GetString();
    ret += "\n";

    return ret;
}