//
// Created by thecharlesblake on 11/13/17.
//

#include <fstream>
#include <sstream>

#include "json_helper.h"

using namespace std;
using namespace rapidjson;

Document json_helper::get_file_json(const string& filename) {
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

void json_helper::json_parse_err(const string& msg) {
    throw runtime_error("Error in JSON doc: " + msg);
}
