//
// Created by thecharlesblake on 11/13/17.
//

#ifndef SOLVITAIRE_UTIL_H
#define SOLVITAIRE_UTIL_H

#include <string>

#include <rapidjson/schema.h>

class json_helper {
public:
    static rapidjson::Document get_file_json(const std::string&);
    static void json_parse_warning(const std::string& msg);
    static void json_parse_err(const std::string& msg);
    static const std::string schema_err_str(const rapidjson::SchemaValidator&);
};


#endif //SOLVITAIRE_UTIL_H
