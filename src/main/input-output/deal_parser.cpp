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

    game_state gs;
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
