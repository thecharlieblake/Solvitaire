//
// Created by thecharlesblake on 10/21/17.
//

#ifndef SOLVITAIRE_DEAL_PARSER_H
#define SOLVITAIRE_DEAL_PARSER_H

#include <vector>

#include <boost/optional.hpp>
#include <rapidjson/document.h>
#include <rapidjson/schema.h>

#include "card.h"

class deal_parser {
public:
    static void parse(rapidjson::Document&, const std::string);
private:
    static const boost::optional<std::string> read_file(const std::string);
    static void gen_schema_doc(rapidjson::Document&);
    static bool to_json(rapidjson::Document&, const char*);
    static bool to_json(rapidjson::Document&, const std::string);
    static const std::string schema_err_str(const rapidjson::SchemaValidator&);
};

#endif //SOLVITAIRE_DEAL_PARSER_H
