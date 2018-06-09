//
// Created by thecharlesblake on 07/06/18.
//

#include "game_state.h"

using std::list;
using std::find;
using std::rbegin;
using std::rend;

// Assesses whether the pile ref that was modified was a tableau, cell or
// reserve pile, and if so makes the relevant function call
void game_state::eval_pile_order(pile::ref pr, bool is_place) {
    if (!original_tableau_piles.empty()
        && pr >= original_tableau_piles[0]
        && pr < original_tableau_piles[0] + original_tableau_piles.size()) {

        eval_pile_order(tableau_piles, pr, is_place);
    } else if (!original_cells.empty()
               && pr >= original_cells[0]
               && pr < original_cells[0] + original_cells.size()) {

        eval_pile_order(cells, pr, is_place);
    } else if (!original_reserve.empty()
               && pr >= original_reserve[0]
               && pr < original_reserve[0] + original_reserve.size()) {

        eval_pile_order(reserve, pr, is_place);
    }
}

// Finds the pile ref in the list and evaluates whether it should be moved to
// maintain the "pile order"
void game_state::eval_pile_order(list<pile::ref>& pile_lst, pile::ref changed_pr,
                                 bool is_place) {
    // Finds the first pile that is larger/smaller than the ref pile
    // (the comp pile).
    // If the card has been placed, search for the ref from the right hand side
    // and if not from the lhs
    if (is_place) {
        auto changed_ref_iter = find(rbegin(pile_lst), rend(pile_lst), changed_pr);
        auto comp_iter = changed_ref_iter;

        while (++comp_iter != rend(pile_lst)
               && piles[*comp_iter] < piles[*changed_ref_iter]);

        if (comp_iter != ++changed_ref_iter) {
            // Places the ref pile before the comp pile
            pile_lst.insert(comp_iter.base(), changed_pr);
            pile_lst.erase(changed_ref_iter.base());
        }
    } else {
        auto changed_ref_iter = find(begin(pile_lst), end(pile_lst), changed_pr);
        auto comp_iter = changed_ref_iter;

        while (++comp_iter != end(pile_lst)
               && piles[*comp_iter] > piles[*changed_ref_iter]);

        if (comp_iter != ++changed_ref_iter) {
            // Places the ref pile before the comp pile
            pile_lst.insert(comp_iter, changed_pr);
            pile_lst.erase(--changed_ref_iter);
        }
    }

#ifndef NDEBUG
    // Makes sure the piles are in order
    if (pile_lst.size() > 1) {
        auto prev = begin(pile_lst);
        auto cur = begin(pile_lst);
        cur++;

        while (cur != end(pile_lst)) {
            assert(piles[*prev] >= piles[*cur]);
            prev++; cur++;
        }
    }
#endif
}
