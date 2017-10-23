#include <iostream>
#include <stdexcept>

#include <boost/program_options.hpp>
#include <rapidjson/document.h>

#include "command_line_helper.h"
#include "card.h"
#include "deal_parser.h"

using namespace rapidjson;

using namespace std;

namespace po = boost::program_options;

bool search(vector<vector<card*>* >, card* hole);

int main(int argc, const char* argv[]) {
    // Parses the command-line options
    command_line_helper clh;
    if (!clh.parse(argc, argv)) {
        return 1;
    }

    for (auto filename : clh.get_input_files()) {
        try {
            Document d;
            deal_parser::parse(d, filename);

            vector<vector<card*>* > tableaux;
            const Value& json_tabx = d["tableau piles"];
            assert(json_tabx.IsArray());

            for (auto& json_tab : json_tabx.GetArray()) {
                vector<card*>* tab = new vector<card*>();

                for (auto& json_card : json_tab.GetArray()) {
                    card* c = new card(json_card.GetString());
                    tab->push_back(c);
                }

                tableaux.push_back(tab);
            }

            bool sol = search(tableaux, new card("AS"));

            cout << (sol ? "Solution Found" : "No Solution");

            for (vector<card*>* tab : tableaux) {
                for (card* c : *tab) {
                    delete c;
                }
                delete tab;
            }
        } catch (runtime_error& e) {
            cerr << "Error: " << e.what() << "\n";
        }


        /*
        // Reads the file into a string
        std::ifstream ifstr(filename);
        std::stringstream buf;
        buf << ifstr.rdbuf();
        string in_json = buf.str();

        if (ifstr.fail()) {
            cerr << "Error: could not read file '" << filename << "'\n";
            //return 1;
        }

        Document sd;
        const char* schema_json = "{\"$schema\":\"http://json-schema.org/draft-04/sc"
                "hema#\",\"definitions\":{\"card\":{\"type\":\"string\",\"pattern"
                "\":\"^(?i)([A|[1-9]|1[0-3]|J|Q|K])([C|D|H|S])$\"}},\"type\":\""
                "object\",\"properties\":{\"tableau piles\":{\"type\":\"array\""
                ",\"items\":{\"type\":\"array\",\"items\":{\"$ref\":\"#/definit"
                "ions/card\"}}},\"hole cards\":{\"type\":\"array\",\"items\":{"
                "\"$ref\":\"#/definitions/card\"}}},\"additionalProperties\":fa"
                "lse,\"anyOf\":[{\"required\":[\"tableau piles\"]}]}";
        if (sd.Parse(schema_json).HasParseError()) {
            // the schema is not a valid JSON.
            assert(false);
        }
        SchemaDocument schema(sd);

        Document d;
        if (d.Parse(in_json.c_str()).HasParseError()) {
            // the input is not a valid JSON.
            cerr << "in not valid json\n";
        }
        SchemaValidator validator(schema);
        if (d.Accept(validator)) {
            vector<vector<card*>* > tableaux;
            const Value& json_tabx = d["tableau piles"];
            assert(json_tabx.IsArray());

            for (auto& json_tab : json_tabx.GetArray()) {
                vector<card*>* tab = new vector<card*>();

                for (auto& json_card : json_tab.GetArray()) {
                    card* c = new card(json_card.GetString());
                    tab->push_back(c);
                }

                tableaux.push_back(tab);
            }

            cout << (search(tableaux, new card("AS")) ? "Solution Found" : "No Solution");

            for (vector<card*>* tab : tableaux) {
                for (card* c : *tab) {
                    delete c;
                }
                delete tab;
            }

        } else {
            // Input JSON is invalid according to the schema
            // Output diagnostic information
            StringBuffer sb;
            validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
            printf("Invalid schema: %s\n", sb.GetString());
            printf("Invalid keyword: %s\n", validator.GetInvalidSchemaKeyword());
            sb.Clear();
            validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
            printf("Invalid document: %s\n", sb.GetString());
        }*/
    }

    return 0;
}

bool search(vector<vector<card*>* > tableaux, card* hole) {
    // If the tableaux are all empty, return true

    // Seaches through each tableau. If the top card is one away from
    // the hole card, puts it in the hole and recurses.
    // If that fails then put the card back and repeat for the next foundation
    // If it succeeds, return true

    // If we get to the end of the function, return false

#ifndef NDEBUG
    string stateRep;
    for (unsigned int i = 0; i < 2; i++) {
        for (vector<card*>* tableau : tableaux) {
            if (tableau->size() > i) {
                cout << *((*tableau)[i]);
            }
            cout << "\t";
        }
        cout << "\n";
    }
    cout << "Hole: " << *hole << "\n";
#endif

    bool allTableauxEmpty = true;
    for (unsigned int i = 0; i < tableaux.size(); i++) {
        auto tableau = tableaux[i];

        if (tableau->empty()) {
            continue;
        } else {
            allTableauxEmpty = false;
        }

        card* topCard = tableau->back();
        if (topCard->get_rank() == hole->get_rank() + 1 || topCard->get_rank() == hole->get_rank() - 1) {
            tableau->pop_back();

            bool success = search(tableaux, topCard);

            if (success) {
                return true;
            } else {
                tableau->push_back(topCard);
            }
        }
    }

    return allTableauxEmpty;
}
