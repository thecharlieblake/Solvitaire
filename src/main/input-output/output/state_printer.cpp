//
// Created by thecharlesblake on 1/3/18.
//

#include "state_printer.h"

using namespace std;

ostream& state_printer::print(ostream& stream, const game_state& gs) {
    if (gs.rules.foundations) {
        state_printer::print_header(stream, "Foundations");
        state_printer::print_top_of_piles(stream, gs.foundations, gs);
    }
    if (gs.rules.cells) {
        state_printer::print_header(stream, "Cells");
        state_printer::print_piles(stream, gs.cells, gs);
    }
    if (gs.rules.tableau_pile_count > 0) {
        state_printer::print_header(stream, "Tableau Piles");
        state_printer::print_piles(stream, gs.tableau_piles, gs);
    }
    if (gs.rules.stock_size > 0) {
        state_printer::print_header(stream, "Stock | Waste");
        state_printer::print_piles(stream, {gs.stock, gs.waste}, gs);
    }
    if (gs.rules.hole) {
        state_printer::print_header(stream, "Hole Card");
        state_printer::print_top_of_pile(stream, gs.hole, gs);
    }
    return stream << "===================================";
}

void state_printer::print_header(ostream& stream, const char* header) {
    stream << "--- " << header << " ";
    size_t pad = 20 - strlen(header);
    for (size_t i = 0; i < pad; i++) {
        stream << '-';
    }
    stream << "\n";
}

void state_printer::print_piles(ostream& stream,
                                const vector<game_state::pile_ref>& pile_rs,
                                const game_state& gs) {
    bool empty_row = false;
    vector<card>::size_type row_idx = 0;

    // Loops through the rows in each pile, starting from the bottom, until
    // one is empty
    while (!empty_row) {
        // Loops through the current row to determine if it is empty
        empty_row = true;
        for (const game_state::pile_ref pile_r : pile_rs) {
            if (gs.piles[pile_r].size() > row_idx) {
                empty_row = false;
                break;
            }
        }
        if (!empty_row || row_idx == 0) {
            // Loops through the current (non-empty) row, and outputs the values
            for (const game_state::pile_ref pile_r : pile_rs) {
                if (gs.piles[pile_r].size() > row_idx) {
                    stream << gs.piles[pile_r][row_idx];
                } else if (row_idx == 0) {
                    stream << "[]";
                }
                stream << "\t";
            }

            stream << "\n";
            row_idx++;
        }
    }
}

void state_printer::print_top_of_piles(ostream& stream,
                                       const vector<game_state::pile_ref>& vp,
                                       const game_state& gs) {
    vector<game_state::pile_ref> top(vp);

    for (game_state::pile_ref p : top) {
        if (gs.piles[p].empty()) {
            stream << "[]";
        } else {
            stream << gs.piles[p].top_card();
        }
        stream << "\t";
    }
    stream << "\n";
}

void state_printer::print_top_of_pile(ostream& stream,
                                      const game_state::pile_ref pile_r,
                                      const game_state& gs) {
    print_top_of_piles(stream, {pile_r}, gs);
}