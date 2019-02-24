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
// Created by thecharlesblake on 11/13/17.
//

#include <fstream>
#include <sstream>

#include "json_helper.h"
#include "../../../../../lib/rapidjson/stringbuffer.h"
#include "../../output/log_helper.h"

using namespace std;
using namespace rapidjson;

using boost::property_tree::ptree;
using boost::property_tree::write_json;

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

void json_helper::print_game_state_as_json(const game_state& gs) {
    ptree pt;
    if (!gs.tableau_piles.empty()) pt.add_child("tableau piles", piles_to_ptree(gs, gs.tableau_piles));
    if (!gs.cells.empty()) pt.add_child("cells", piles_to_ptree(gs, gs.cells));
    if (!gs.reserve.empty()) pt.add_child("reserve", piles_to_ptree(gs, gs.reserve));
    if (!gs.foundations.empty()) pt.add_child("foundations", piles_to_ptree(gs, gs.foundations));
    if (!gs.sequences.empty()) pt.add_child("sequences", piles_to_ptree(gs, gs.sequences));
    if (!gs.accordion.empty()) pt.add_child("accordion", piles_to_ptree(gs, gs.accordion));
    if (gs.rules.stock_size > 0) {
        pt.add_child("stock", pile_to_ptree(gs.piles[gs.stock]));
        if (gs.rules.stock_deal_t == sol_rules::stock_deal_type::WASTE) {
            pt.add_child("waste", pile_to_ptree(gs.piles[gs.waste]));
        }
    }
    if (gs.rules.hole) pt.add_child("hole", card_to_ptree(gs.piles[gs.hole][0]));

    write_json (cout, pt);
}

ptree json_helper::piles_to_ptree(const game_state& gs, const std::list<pile::ref>& piles) {
    ptree pt;
    for (pile::ref pr : piles) {
        pt.push_back(make_pair("", pile_to_ptree(gs.piles[pr])));
    }
    return pt;
}

ptree json_helper::piles_to_ptree(const game_state& gs, const std::vector<pile::ref>& piles) {
    ptree pt;
    for (pile::ref pr : piles) {
        pt.push_back(make_pair("", pile_to_ptree(gs.piles[pr])));
    }
    return pt;
}

ptree json_helper::pile_to_ptree(const pile& pile) {
    ptree pt;
    for (pile::size_type i = pile.size(); i-->0; ) {
        pt.push_back(make_pair("", card_to_ptree(pile[i])));
    }
    return pt;
}

ptree json_helper::card_to_ptree(const card& card) {
    ptree pt;
    pt.put("", card.to_string());
    return pt;
}
