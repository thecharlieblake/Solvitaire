//
// Created by thecharlesblake on 07/06/18.
//

#include "game_state.h"


typedef sol_rules::build_policy pol;

using std::max;


bool game_state::is_valid_auto_foundation_move(pile::ref target_pile) const {
    if (rules.foundations_comp_piles)
        return false;
    else if (streamliners || rules.build_pol == pol::NO_BUILD || rules.build_pol == pol::SAME_SUIT)
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

// Returns a dominance move if one is available
boost::optional<move> game_state::get_dominance_move() const {
#ifndef NO_AUTO_FOUNDATIONS
    // If there are 2 decks or no foundations, return
    if (!rules.foundations || rules.two_decks)
        return boost::none;

    // Cycles through the piles and sees if any cards can be automatically moved
    // to the foundations
    for (pile::ref pr = 0; pr < piles.size(); pr++) {
        // Don't move foundation cards, hole or stock cards to the foundations
        if ((pr >= foundations.front() && pr <= foundations.back())
            || (rules.hole && pr == hole)
            || (rules.stock_size > 0 && pr == stock)
            || (piles[pr].empty())) {
            continue;
        }

        card c = piles[pr].top_card();
        pile::ref target_foundation = foundations[c.get_suit()];

        // If the card is the right rank and the auto-move boolean is true, then
        // returns the move
        card::rank_t target_rank = piles[target_foundation].empty()
                                   ? card::rank_t(1) :
                                   piles[target_foundation].top_card().get_rank()
                                   + card::rank_t(1);
        if (target_rank == c.get_rank() &&
            auto_foundation_moves[c.get_suit()]) {
            move m(move::mtype::dominance, pr, target_foundation);

            // If this dominance move reveals a card, adds this to the move
            if (piles[pr].size() > 1 && piles[pr][1].is_face_down())
                m.make_reveal_move();

            return m;
        }
    }
#endif

    return boost::none;
}

bool game_state::is_ordered_pile(pile::ref pr) const {
    if (piles[pr].size() != rules.max_rank) return false;

    card::suit_t s = piles[pr][0].get_suit();
    for (card::rank_t r = 0; r < rules.max_rank; r++) {
        if (piles[pr][r] != card(s, card::rank_t(r + 1))) return false;
    }
    return true;
}

// For games where the foundations can be removed from, this dominance blocks
// the foundations being removed from if the card that were to be removed would
// go 'automatically' up
bool game_state::dominance_blocks_foundation_move(pile::ref target_pile) {
    assert(!piles[target_pile].empty());

    card target_card = piles[target_pile].top_card();
    piles[target_pile].take();

    bool blocked = is_valid_auto_foundation_move(target_pile);
    piles[target_pile].place(target_card);

    return blocked;
}

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
