//
// Created by thecharlesblake on 07/06/18.
//

#include "game_state.h"


typedef sol_rules::build_policy pol;

using std::max;

// Decides which 'auto foundation' booleans should be 'true' currently
void game_state::update_auto_foundation_moves(pile::ref target_pile) {
    // If there are no foundations, or this is not a foundation move, do nothing
    if (!rules.foundations
        || target_pile < foundations.front()
        || target_pile > foundations.back()) {
        return;
    }

    // Turns the 'auto' boolean for the target foundation false

    for (pile::ref pr : foundations) {
        auto_foundation_moves[pr] = is_valid_auto_foundation_move(pr);
    }
}

bool game_state::is_valid_auto_foundation_move(pile::ref target_pile) const {
#ifndef AUTO_FOUNDATIONS_STREAMLINER
    if (rules.build_pol == pol::NO_BUILD || rules.build_pol == pol::SAME_SUIT)
#endif
        return true;

    card::suit_t target_suit(target_pile - foundations.front());
    card::rank_t target_rank = piles[target_pile].empty()
                               ? card::rank_t(0)
                               : piles[target_pile].top_card().get_rank();
    // The card to be moved up
    card move_up_card(target_suit, card::rank_t(target_rank + 1));

    // The max difference between the 'move up' card and the foundations of the
    // same colour and the other colour
    int same_rank_diff = 0;
    int other_rank_diff = 0;
    for (pile::ref pr : foundations) {
        if (pr == target_pile) continue;

        card::suit_t s(pr - foundations.front());
        card::rank_t r = piles[pr].empty() ? card::rank_t(0)
                                           : piles[pr].top_card().get_rank();
        card c(s, r);

        auto rank_diff = int(move_up_card.get_rank() - r);
        if (move_up_card.get_suit() == c.get_suit()
            || (rules.build_pol == pol::RED_BLACK
                && move_up_card.get_colour() == c.get_colour())) {
            same_rank_diff = rank_diff;
        } else {
            other_rank_diff = max(other_rank_diff, rank_diff);
        }
    }

    bool other_within_1 = other_rank_diff <= 1;
    bool other_within_2 = other_rank_diff <= 2;
    bool same_within_3 = same_rank_diff <= 3;

    if (rules.build_pol == pol::RED_BLACK) {
        return other_within_1 || (other_within_2 && same_within_3);
    } else {
        assert(rules.build_pol == pol::ANY_SUIT);
        return other_within_2;
    }
}
