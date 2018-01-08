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

    // If there are cell piles, creates the relevant cell vectors
    if (rules.cells > 0) {
        for (uint8_t i = 0; i < rules.cells; i++) {
            piles.emplace_back();
            cells.push_back(static_cast<pile_ref>(piles.size() - 1));
        }
    }

    // If there is a stock & waste, creates piles
    if (rules.stock_size > 0) {
        piles.emplace_back();
        stock = static_cast<pile_ref>(piles.size() - 1);
        piles.emplace_back();
        waste = static_cast<pile_ref>(piles.size() - 1);
    }

    // If there is a reserve, creates piles
    if (rules.reserve_size > 0) {
        uint8_t pile_count = rules.reserve_stacked ?
                             uint8_t(1) : rules.reserve_size;
        for (uint8_t i = 0; i < pile_count; i++) {
            piles.emplace_back();
            reserve.push_back(static_cast<pile_ref>(piles.size() - 1));
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

    // If the foundations begin filled, then fills them
    if (rules.foundations_init_card) {
        for (uint8_t f_idx = 0; f_idx < 4; f_idx++) {
            card c = card(card::to_suit(f_idx), 1);

            deck.erase(find(begin(deck), end(deck), c));
            piles[foundations[f_idx]].place(c);
        }
    }

    // If there is a stock, deals to it and set up a waste pile too
    if (rules.stock_size > 0) {
        for (unsigned int i = 0; i < rules.stock_size; i++) {
            piles[stock].place(deck.back());
            deck.pop_back();
        }
    }

    // If there is a reserve, deals to it.
    // We treat a regular reserve like multiple single-card piles,
    // but a stacked reserve as a single multiple-card pile.
    if (rules.reserve_size > 0) {
        for (unsigned int i = 0; i < rules.reserve_size; i++) {
            int idx = rules.reserve_stacked ? 0 : i;
            piles[reserve[idx]].place(deck.back());
            deck.pop_back();
        }
    }

    // Deals to the tableau piles (row-by-row)
    for (int t = 0; !deck.empty(); t++) {
        card c = deck.back();

        // Add the randomly generated card to the tableau piles
        auto p = t % tableau_piles.size();

        // If we are doing a diagonal deal, each row should have one fewer card.
        // Leftover cards are dealt normally in full rows.
        auto row_idx = t / tableau_piles.size();
        if (rules.diagonal_deal && row_idx < tableau_piles.size()) {
            p = tableau_piles.size()-p-1;
            pile_ref tableau_pile = tableau_piles[p];

            if (p >= row_idx) {
                piles[tableau_pile].place(c);
                deck.pop_back();
            }
        } else {
            pile_ref tableau_pile = tableau_piles[p];
            piles[tableau_pile].place(c);
            deck.pop_back();
        }
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

game_state::move::move(pile_ref f, pile_ref t, pile::size_type i)
        : from(f), to(t), count(i) {
    assert(i >= 1);
}


/////////////////////////
// MODIFYING FUNCTIONS //
/////////////////////////

void game_state::make_move(const move m) {
    // If this is not a built-pile move
    if (m.count == 1) {
        piles[m.to].place(piles[m.from].take());
    } else {
        // Adds the cards to the 'to' pile
        for (auto pile_idx = m.count; pile_idx-- > 0;) {
            piles[m.to].place(piles[m.from][pile_idx]);
        }

        // Removes the cards from the 'from' pile
        for (uint8_t rem_count = 0; rem_count < m.count; rem_count++) {
            piles[m.from].take();
        }
    }
}

void game_state::undo_move(const move m) {
    // If this is not a built-pile move
    if (m.count == 1) {
        piles[m.from].place(piles[m.to].take());
    } else {
        // Adds the cards to the 'from' pile
        for (auto pile_idx = m.count; pile_idx-- > 0;) {
            piles[m.from].place(piles[m.to][pile_idx]);
        }

        // Removes the cards from the 'to' pile
        for (uint8_t rem_count = 0; rem_count < m.count; rem_count++) {
            piles[m.to].take();
        }
    }
}


////////////////////////
// INSPECT GAME STATE //
////////////////////////

bool game_state::is_solved() const {
    if (rules.hole) {
        return piles[hole].size() == rules.max_rank * 4;
    } else {
        for (auto f : foundations) {
            if (piles[f].size() != rules.max_rank) {
                return false;
            }
        }
        return true;
    }
}

const vector<pile>& game_state::get_data() const {
    return piles;
}

bool operator==(const game_state& a, const game_state& b) {
    return a.tableau_piles == b.tableau_piles
           && a.foundations == b.foundations
           && a.hole == b.hole;
}


//////////////////////////////////
// STATIC VARIABLES AND MEMBERS //
//////////////////////////////////

game_state::pile_ref game_state::PILE_REF_MAX = 255;

game_state::move game_state::null_move() {
    return {PILE_REF_MAX, 0, PILE_REF_MAX};
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
