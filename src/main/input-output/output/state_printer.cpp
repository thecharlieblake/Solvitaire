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
// Created by thecharlesblake on 1/3/18.
//

#include "state_printer.h"

using std::ostream;
using std::vector;
using std::list;

typedef sol_rules::stock_deal_type sdt;

ostream& state_printer::print(ostream& stream, const game_state& gs) {
    if (gs.rules.foundations_present) {
        state_printer::print_header(stream, "Foundations");
        state_printer::print_top_of_piles(stream, gs.foundations, gs);
    }
    if (gs.rules.cells) {
        state_printer::print_header(stream, "Cells");
        state_printer::print_piles(stream, gs.original_cells, gs);
    }
    if (gs.rules.tableau_pile_count > 0) {
        state_printer::print_header(stream, "Tableau Piles");
        state_printer::print_piles(stream, gs.original_tableau_piles, gs);
    }
    if (gs.rules.reserve_size > 0) {
        if (gs.rules.reserve_stacked) {
            state_printer::print_header(stream, "Reserve (Stacked)");
        } else {
            state_printer::print_header(stream, "Reserve");
        }
        state_printer::print_piles(stream, gs.original_reserve, gs);
    }
    if (gs.rules.stock_size > 0) {
        if (gs.rules.stock_deal_t == sdt::WASTE) {
            state_printer::print_header(stream, "Stock | Waste");
            state_printer::print_piles(stream, {gs.stock, gs.waste}, gs);
        } else {
            state_printer::print_header(stream, "Stock");
            state_printer::print_piles(stream, {gs.stock}, gs);
        }
    }
    if (gs.rules.hole) {
        state_printer::print_header(stream, "Hole Card");
        state_printer::print_top_of_pile(stream, gs.hole, gs);
    }
    if (gs.rules.sequence_count > 0) {
        state_printer::print_header(stream, "Sequences");
        state_printer::print_sequences(stream, gs.sequences, gs);
    }
    if (gs.rules.accordion_size > 0) {
        state_printer::print_header(stream, "Accordion");
        state_printer::print_accordion(stream, gs.accordion, gs);
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
                                const vector<pile::ref>& pile_rs,
                                const game_state& gs) {
    bool empty_row = false;
    pile::size_type row_idx = 0;

    // Loops through the rows in each pile, starting from the bottom, until
    // one is empty
    while (!empty_row) {
        // Loops through the current row to determine if it is empty
        empty_row = true;
        for (const pile::ref pile_r : pile_rs) {
            if (gs.piles[pile_r].size() > row_idx) {
                empty_row = false;
                break;
            }
        }
        if (!empty_row || row_idx == 0) {
            // Loops through the current (non-empty) row, and outputs the values
            for (const pile::ref pile_r : pile_rs) {
                const pile& p = gs.piles[pile_r];
                if (p.size() > row_idx) {
                    print_card(stream, p[p.size() - 1 - row_idx]);
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

void state_printer::print_sequences(ostream& stream,
                                const vector<pile::ref>& seq_rs,
                                const game_state& gs) {
    for (auto s : seq_rs) {
        stream << "\n";
        auto& pile = gs.piles[s];
        for (auto i = pile.size(); i --> 0 ;) {
            card c = pile[i];
            if (c == "AS")
                stream << "__";
            else
                print_card(stream, pile[i]);
            stream << " ";
        }
        stream << "\n";
    }
    stream << "\n";
}

void state_printer::print_top_of_piles(ostream& stream,
                                       const vector<pile::ref>& vp,
                                       const game_state& gs) {
    vector<pile::ref> top(vp);

    for (pile::ref p : top) {
        if (gs.piles[p].empty()) {
            stream << "[]";
        } else {
            print_card(stream, gs.piles[p].top_card());
        }
        stream << "\t";
    }
    stream << "\n";
}

void state_printer::print_accordion(ostream& stream,
                                       const list<pile::ref>& vp,
                                       const game_state& gs) {
    for (pile::ref p : vp) {
        print_card(stream, gs.piles[p].top_card());
        stream << "\t";
    }
    stream << "\n";
}

void state_printer::print_top_of_pile(ostream& stream,
                                      const pile::ref pile_r,
                                      const game_state& gs) {
    print_top_of_piles(stream, {pile_r}, gs);
}

void state_printer::print_card(std::ostream& s, const card c) {
    s << c.to_string();
}

void state_printer::print_move(std::ostream& s, const move m) {
    s << "move(";
    switch (m.type) {
        case move::mtype::regular:
            s << "regular";
            break;
        case move::mtype::built_group:
            s << "built group";
            break;
        case move::mtype::stock_k_plus:
            s << "stock k-plus";
            break;
        case move::mtype::stock_to_all_tableau:
            s << "stock-to-all-tableau";
            break;
        case move::mtype::sequence:
            s << "sequence";
            break;
        case move::mtype::accordion:
            s << "accordion";
            break;
        case move::mtype::null:
            s << "null";
            break;
    }
    s << ", "
      << int(m.from)
      << ", "
      << int(m.to)
      << ", "
      << int(m.count)
      << ", "
      << (m.reveal_move ? "true" : "false") 
      << ", "
      << (m.dominance_move ? "dominance" : "regular")
      << ")\n";
}
