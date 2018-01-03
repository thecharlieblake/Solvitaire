//
// Created by thecharlesblake on 12/26/17.
//

#include <rapidjson/document.h>

#include "test_helper.h"
#include "../main/game/game_state.h"
#include "../main/solver/solver.h"
#include "../main/input-output/json_helper.h"

using namespace rapidjson;
using namespace std;

bool test_helper::is_solvable(const std::string& input_file, const std::string& preset_type) {
    const Document in_doc = json_helper::get_file_json(input_file);
    const sol_rules rules = sol_rules::from_preset(preset_type);

    game_state gs(rules, in_doc);
    solver sol(gs, rules);

    return sol.run() ? true : false;
}
