//
// Created by thecharlesblake on 10/24/17.
//

#include <vector>
#include <ostream>
#include <random>
#include <algorithm>
#include <functional>

#include <rapidjson/document.h>
#include <boost/functional/hash.hpp>

#include "game_state.h"
#include "../input-output/input/json-parsing/deal_parser.h"
#include "../input-output/output/state_printer.h"

using namespace std;
using namespace rapidjson;
using namespace boost;

typedef sol_rules::build_order ord;
typedef sol_rules::build_policy pol;
typedef sol_rules::spaces_policy s_pol;


//////////////////
// CONSTRUCTORS //
//////////////////

// A private constructor used by both of the public ones. Initializes all of the
// piles and pile refs specified by the rules
game_state::game_state(const sol_rules& s_rules) :
        rules(s_rules),
        stock(PILE_REF_MAX),
        waste(PILE_REF_MAX),
        hole(PILE_REF_MAX) {

    // If there is a hole, creates pile
    if (rules.hole) {
        piles.emplace_back();
        hole = static_cast<pile_ref>(piles.size() - 1);
    }

    // If there are foundation piles, creates the relevant pile vectors
    if (rules.foundations) {
        for (uint8_t i = 0; i < 4; i++) {
            piles.emplace_back();
            foundations.push_back(static_cast<pile_ref>(piles.size() - 1));
        }
    }

    // If there is a stock & waste, creates piles
    if (rules.stock_size > 0) {
        piles.emplace_back();
        stock = static_cast<pile_ref>(piles.size() - 1);
        piles.emplace_back();
        waste = static_cast<pile_ref>(piles.size() - 1);
    }

    // If there are cell piles, creates the relevant cell vectors
    if (rules.cells > 0) {
        for (uint8_t i = 0; i < rules.cells; i++) {
            piles.emplace_back();
            cells.push_back(static_cast<pile_ref>(piles.size() - 1));
        }
    }

    // Creates the tableau piles
    for (uint8_t i = 0; i < rules.tableau_pile_count; i++) {
        piles.emplace_back();
        tableau_piles.push_back(static_cast<pile_ref>(piles.size() - 1));
    }
}

// Constructs an initial game state from a JSON doc
game_state::game_state(const sol_rules& s_rules, const Document& doc) :
        game_state(s_rules) {
    deal_parser::parse(*this, doc);
}

// Constructs an initial game state from a seed
game_state::game_state(const sol_rules& s_rules, int seed) :
        game_state(s_rules) {

    vector<card> deck = gen_shuffled_deck(seed, rules.max_rank);

    // If there is a hole, moves the ace of spades to it
    if (rules.hole) {
        deck.erase(find(begin(deck), end(deck), card("AS")));
        piles[hole].place(card("AS"));
    }

    // If there is a stock, deals to it and set up a waste pile too
    if (rules.stock_size > 0) {
        for (unsigned int i = 0; i < rules.stock_size; i++) {
            piles[stock].place(deck.back());
            deck.pop_back();
        }
    }

    // Deals to the tableau piles (row-by-row)
    for (int t = 0; !deck.empty(); t++) {
        card c = deck.back();
        deck.pop_back();

        // Add the randomly generated card to the tableau piles
        pile_ref tableau_pile = tableau_piles[t % tableau_piles.size()];
        piles[tableau_pile].place(c);
    }
}

// Generates a randomly ordered vector of cards
vector<card> game_state::gen_shuffled_deck(int seed, int max_rank = 13) {
    vector<int> values;
    vector<int*> v_ptrs;
    for (int i = 0; i < max_rank * 4; i++) {
        values.emplace_back(i);
    }
    for (int i = 0; i < max_rank * 4; i++) {
        v_ptrs.emplace_back(&values[i]);
    }

    // Randomly shuffle the pointers
    auto rng = default_random_engine(seed);
    shuffle(begin(v_ptrs), end(v_ptrs), rng);

    vector<card> deck;
    for (int *i : v_ptrs) {
        auto r = static_cast<card::rank_t>(((*i) % max_rank) + 1);
        card::suit_t s;
        switch ((*i) / max_rank) {
            case 0 :  s = card::suit_t::Clubs;
                break;
            case 1 :  s = card::suit_t::Diamonds;
                break;
            case 2 :  s = card::suit_t::Hearts;
                break;
            case 3 :  s = card::suit_t::Spades;
                break;
            default : assert(false);
                s = card::suit_t::Clubs;
                break;
        }

        deck.emplace_back(card(s, r));
    }

    return deck;
}


/////////////////////////
// MODIFYING FUNCTIONS //
/////////////////////////

void game_state::make_move(const move m) {
    piles[m.second].place(piles[m.first].take());
}

void game_state::undo_move(const move m) {
    piles[m.first].place(piles[m.second].take());
}


///////////////////////////
// LEGAL MOVE GENERATION //
///////////////////////////

vector<game_state::move> game_state::get_legal_moves() const {
    // The next legal moves
    vector<move> moves;

    // Cycles through each pile which we may be able to remove a card from
    for (pile_ref rem_ref = 0; rem_ref < piles.size(); rem_ref++) {
        // Never removes a card from the hole, the waste, or an empty pile
        if (rem_ref == hole
            || rem_ref == waste
            || piles[rem_ref].empty()) continue;

        // Stock cards can only be moved to the waste
        if (stock != PILE_REF_MAX && rem_ref == stock) {
            moves.emplace_back(stock, waste);
            continue;
        }

        // For each tableau pile, check if moving from the rem pile is a valid
        // tableau move according to the rules
        for (auto add_ref : tableau_piles) {
            if (is_valid_tableau_move(rem_ref, add_ref)) {
                moves.emplace_back(rem_ref, add_ref);
            }
        }

        // Any card can be moved to a cell, so long as the cell isn't full
        for (auto add_ref : cells) {
            if (add_ref != rem_ref && piles[add_ref].empty()) {
                moves.emplace_back(rem_ref, add_ref);
            }
        }

        // Any card can be added to the waste
        if (waste != PILE_REF_MAX && waste != rem_ref) {
            moves.emplace_back(rem_ref, waste);
        }

        // For each foundation pile, check if moving from the rem pile is a valid
        // move according to the rules
        for (auto add_ref : foundations) {
            if (is_valid_foundations_move(rem_ref, add_ref)) {
                moves.emplace_back(rem_ref, add_ref);
            }
        }

        // Does the same for the hole
        if (hole != PILE_REF_MAX && is_valid_hole_move(rem_ref)) {
            moves.emplace_back(rem_ref, hole);
        }
    }

    return moves;
}

// If a tableau pile is empty and the space policy is
// no-build, do not add. Otherwise any card with the right
// suit/rank can be moved to a tableau
bool game_state::is_valid_tableau_move(const pile_ref rem_ref,
                                       const pile_ref add_ref) const {
    if (rem_ref == add_ref) return false;

    else if (rules.build_pol == pol::NO_BUILD) return false;

    else if (piles[add_ref].empty()) {
        return rules.spaces_pol == s_pol::ANY;
    }

    card rem_c = piles[rem_ref].top_card();
    card add_c = piles[add_ref].top_card();

    // Checks violation of same suit policy
    if (rules.build_pol == pol::SAME_SUIT
        && rem_c.get_suit() != add_c.get_suit()) {
        return false;
    }

        // Checks violation of red-black suit policy
    else if (rules.build_pol == pol::RED_BLACK
             && rem_c.get_colour() == add_c.get_colour()) {
        return false;
    }

        // Checks rank
    else if (rules.build_ord == ord::DESCENDING
             && rem_c.get_rank() + 1 != add_c.get_rank()) {
        return false;
    } else if (rules.build_ord == ord::ASCENDING
               && rem_c.get_rank() - 1 != add_c.get_rank()) {
        return false;
    } else {
        return true;
    }
}

bool game_state::is_valid_foundations_move(const pile_ref rem_ref,
                                       const pile_ref add_ref) const {
    if (rem_ref == add_ref) return false;

    card rem_c = piles[rem_ref].top_card();

    // Checks violation of same suit policy
    if (rem_c.get_suit() != card::to_suit(add_ref - foundations.front())) {
        return false;
    }

        // Checks rank
    else {
        card::rank_t add_rank = piles[add_ref].empty()
                                ? card::rank_t(0)
                                : piles[add_ref].top_card().get_rank();
        return rem_c.get_rank() - 1 == add_rank;
    }
}

bool game_state::is_valid_hole_move(const pile_ref rem_ref) const {
    if (rem_ref == hole) return false;
    card::rank_t rank = piles[rem_ref].top_card().get_rank();
    card::rank_t hole_rank = piles[hole].top_card().get_rank();
    return rank + 1 == hole_rank
           || rank - 1 == hole_rank
           || (rank == rules.max_rank && hole_rank == 1)
           || (rank == 1 && hole_rank == rules.max_rank);
}


////////////////////////
// INSPECT GAME STATE //
////////////////////////

bool game_state::is_solved() const {
    for (auto f : foundations) {
        if (piles[f].size() != rules.max_rank) {
            return false;
        }
    }

    return !rules.hole || piles[hole].size() == rules.max_rank * 4;
}

const vector<pile>& game_state::get_data() const {
    return piles;
}

bool operator==(const game_state& a, const game_state& b) {
    return a.tableau_piles == b.tableau_piles
           && a.foundations == b.foundations
           && a.hole == b.hole;
}


////////////////////
// HASH FUNCTIONS //
////////////////////

size_t hash_value(game_state const& gs) {
    size_t seed = 0;

    if (gs.rules.foundations) {
        hash_combine(seed, gs.foundations);
    }
    if (gs.rules.cells) {
        hash_combine(seed, gs.cells);
    }
    if (gs.rules.tableau_pile_count > 0) {
        hash_combine(seed, gs.tableau_piles);
    }

    if (gs.rules.stock_size > 0) {
        hash_combine(seed, gs.piles[gs.stock]);
        hash_combine(seed, gs.piles[gs.waste]);
    }
    if (gs.rules.hole) {
        hash_combine(seed, gs.piles[gs.hole]);
    }

    return seed;
}

size_t hash_value(vector<pile> const& vp) {
    return hash_range(begin(vp), end(vp));
}


///////////
// PRINT //
///////////

ostream& operator<< (ostream& str, const game_state& gs) {
    return state_printer::print(str, gs);
}
