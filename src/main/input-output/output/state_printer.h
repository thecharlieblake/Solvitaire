//
// Created by thecharlesblake on 1/3/18.
//

#ifndef SOLVITAIRE_STATE_PRINT_H
#define SOLVITAIRE_STATE_PRINT_H

#include <ostream>

#include "../../game/search-state/game_state.h"

class state_printer {
public:
    static std::ostream& print(std::ostream&, const game_state&);
    static void print_move(std::ostream&, move);

private:
    static void print_header(std::ostream&, const char*);
    static void print_piles(std::ostream&,
                            const std::vector<pile::ref>&,
                            const game_state&);
    static void print_sequences(std::ostream&,
                            const std::vector<pile::ref>&,
                            const game_state&);
    static void print_top_of_piles(std::ostream&,
                                   const std::vector<pile::ref>&,
                                   const game_state&);
    static void print_top_of_pile(std::ostream&, pile::ref,
                                  const game_state&);
    static void print_card(std::ostream&, card);
};

#endif //SOLVITAIRE_STATE_PRINT_H
