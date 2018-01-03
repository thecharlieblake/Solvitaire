//
// Created by thecharlesblake on 1/3/18.
//

#ifndef SOLVITAIRE_STATE_PRINT_H
#define SOLVITAIRE_STATE_PRINT_H

#include <ostream>

#include "../../game/game_state.h"

class state_printer {
public:
    static std::ostream& print(std::ostream&, const game_state&);

private:
    static void print_header(std::ostream&, const char*);
    static void print_piles(std::ostream&,
                            const std::vector<game_state::pile_ref>&,
                            const game_state&);
    static void print_top_of_piles(std::ostream&,
                                   const std::vector<game_state::pile_ref>&,
                                   const game_state&);
    static void print_top_of_pile(std::ostream&, game_state::pile_ref,
                                  const game_state&);
};

#endif //SOLVITAIRE_STATE_PRINT_H
