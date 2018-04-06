//
// Created by thecharlesblake on 11/13/17.
//

#include <fstream>
#include <sstream>

#include "json_helper.h"
#include "../../../../../lib/rapidjson/stringbuffer.h"
#include "../../output/log_helper.h"

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
    string err_msg = "Error in JSON doc: " + msg;
    LOG_DEBUG(err_msg);
    throw runtime_error(err_msg);
}

void json_helper::json_parse_warning(const string& msg) {
    LOG_WARNING("Error in JSON doc: " + msg);
}

const string json_helper::schema_err_str(const SchemaValidator& validator) {
    string ret = "Input JSON failed to match the required schema. Schema Validator Error = ";

    StringBuffer sb;
    validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
    ret += validator.GetInvalidSchemaKeyword();
    ret += ": ";

    sb.Clear();
    validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);

    ret += sb.GetString();
    return ret;
}
