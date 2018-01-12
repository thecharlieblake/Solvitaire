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
#include "sol_rules.h"

using namespace std;
using namespace rapidjson;
using namespace boost;

typedef sol_rules::build_policy pol;
typedef sol_rules::stock_deal_type sdt;


//////////////////
// CONSTRUCTORS //
//////////////////

// A private constructor used by both of the public ones. Initializes all of the
// piles and pile refs specified by the rules
game_state::game_state(const sol_rules& s_rules) :
        rules(s_rules) {

    // If there is a hole, creates pile
    if (rules.hole) {
        piles.emplace_back();
        hole = static_cast<pile_ref>(piles.size() - 1);
    }

    // If there are foundation piles, creates the relevant pile vectors
    if (rules.foundations) {
        for (uint8_t i = 0; i < 4*(rules.two_decks ? 2:1); i++) {
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

    // If there is a stock, creates a pile
    if (rules.stock_size > 0) {
        piles.emplace_back();
        stock = static_cast<pile_ref>(piles.size() - 1);

        // If the stock deals to a waste, create the waste
        if (rules.stock_deal_t == sdt::WASTE) {
            piles.emplace_back();
            waste = static_cast<pile_ref>(piles.size() - 1);
        }
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
    vector<card> deck = gen_shuffled_deck(seed, rules.max_rank, rules.two_decks);

    // If there is a hole, moves the ace of spades to it
    if (rules.hole) {
        deck.erase(find(begin(deck), end(deck), card("AS")));
        piles[hole].place(card("AS"));
    }

    // If the foundations begin filled, then fills them
    if (rules.foundations_init_card) {
        for (uint8_t f_idx = 0; f_idx < 4*(rules.two_decks ? 2:1); f_idx++) {
            card c = card(card::to_suit(f_idx % uint8_t(4)), 1);

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

    // The size of all piles must equal the deck size
    int piles_sz = 0;
    for (auto& p : piles) piles_sz += p.size();
    if (piles_sz != rules.max_rank * (rules.two_decks ? 8:4)) {
        throw runtime_error("Error: incorrect number of cards in starting piles");
    }
}

// Generates a randomly ordered vector of cards
vector<card> game_state::gen_shuffled_deck(int seed, card::rank_t max_rank,
                                           bool two_decks) {
    vector<uint8_t> values;
    vector<uint8_t*> v_ptrs;
    for (int i = 0; i < max_rank * 4; i++) {
        values.emplace_back(i);
        if (two_decks) values.emplace_back(i);
    }
    for (auto& v : values) {
        v_ptrs.emplace_back(&v);
    }

    // Randomly shuffle the pointers
    auto rng = default_random_engine(seed);
    shuffle(begin(v_ptrs), end(v_ptrs), rng);

    vector<card> deck;
    for (uint8_t *i : v_ptrs) {
        auto r = static_cast<card::rank_t>(((*i) % max_rank) + 1);
        card::suit_t s = card::to_suit((*i) / max_rank);
        deck.emplace_back(card(s, r));
    }

    assert(deck.size() == pile::size_type(max_rank * (two_decks ? 8 : 4)));
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
    assert(m.from < piles.size());
    assert(m.to < piles.size());

    // Handles special stock-to-tableau-piles move
    if (rules.stock_deal_t == sdt::TABLEAU_PILES && m.from == stock) {
        for (pile_ref tab_pr = tableau_piles.front();
             tab_pr < tableau_piles.front() + m.count;
             tab_pr++) {
            piles[tab_pr].place(piles[stock].take());
        }
    }
    // If this is not a built-pile move
    else if (m.count == 1) {
        piles[m.to].place(piles[m.from].take());
    } else {
        assert(m.count <= rules.max_rank);
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
    assert(m.from < piles.size());
    assert(m.to < piles.size());

    // Handles special stock-to-tableau-piles move
    if (rules.stock_deal_t == sdt::TABLEAU_PILES && m.from == stock) {
        for (pile_ref tab_pr = tableau_piles.front() + m.count;
             tab_pr-- > tableau_piles.front();
                ) {
            piles[stock].place(piles[tab_pr].take());
        }
    }
    // If this is not a built-pile move
    else if (m.count == 1) {
        piles[m.from].place(piles[m.to].take());
    } else {
        assert(m.count <= rules.max_rank);
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
    bool solved = true;
    if (rules.hole) {
        solved = piles[hole].size()
               == rules.max_rank * 4 * (rules.two_decks ? 2 : 1);
    } else {
        for (auto f : foundations) {
            if (piles[f].size() != rules.max_rank) {
                solved = false;
            }
        }
    }

    // Runs some alternative checks in debug mode to make sure the game state
    // is consistent
#ifndef NDEBUG
    bool rest_empty = true;
    for (pile_ref pr = 0; pr < piles.size(); pr++) {
        // All piles other than the hole must be empty
        if (rules.hole) {
            if (pr != hole && !piles[pr].empty()) {
                rest_empty = false;
                break;
            }
        }
            // All piles other than the foundations must be empty
        else {
            bool non_foundation_ref =
                    pr < foundations.front()
                    || pr >= foundations.front() + foundations.size();
            if (non_foundation_ref && !piles[pr].empty()) {
                rest_empty = false;
                break;
            }
        }
    }
    assert(solved == rest_empty);
#endif

    return solved;
}

const vector<pile>& game_state::get_data() const {
    return piles;
}

bool operator==(const game_state& a, const game_state& b) {
    return a.tableau_piles == b.tableau_piles
           && a.foundations == b.foundations
           && a.hole == b.hole;
}


///////////
// PRINT //
///////////

ostream& operator<< (ostream& str, const game_state& gs) {
    return state_printer::print(str, gs);
}
