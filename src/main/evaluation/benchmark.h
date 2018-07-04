//
// Created by thecharlesblake on 4/5/18.
//

#ifndef SOLVITAIRE_BENCHMARK_H
#define SOLVITAIRE_BENCHMARK_H

#include <chrono>

#include "../game/sol_rules.h"
#include "../game/search-state/game_state.h"

class benchmark {
public:
    static void run(const sol_rules &rules, uint64_t, game_state::streamliner_options);
};


#endif //SOLVITAIRE_BENCHMARK_H
