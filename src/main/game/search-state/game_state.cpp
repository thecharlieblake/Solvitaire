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
// Created by thecharlesblake on 10/24/17.
//

#include <vector>
#include <ostream>
#include <algorithm>
#include <functional>

#include <boost/functional/hash.hpp>
#include <boost/optional.hpp>
#include <boost/random.hpp>

#include "game_state.h"
#include "document.h"
#include "../../input-output/input/json-parsing/deal_parser.h"
#include "../../input-output/output/state_printer.h"
#include "../../input-output/output/log_helper.h"
#include "../move.h"
#include "../sol_rules.h"

using namespace rapidjson;
using std::vector;
using std::list;
using std::find;
using std::begin;
using std::end;
using std::rbegin;
using std::rend;
using std::runtime_error;
using std::max;
using std::ostream;
using std::mt19937;
using std::string;

typedef sol_rules::build_policy pol;
typedef sol_rules::stock_deal_type sdt;
typedef sol_rules::face_up_policy fu;
typedef game_state::streamliner_options sos;
typedef sol_rules::foundations_init_type fit;

//////////////////
// CONSTRUCTORS //
//////////////////

// A private constructor used by both of the public ones. Initializes all of the
// piles and pile refs specified by the rules
game_state::game_state(const sol_rules& s_rules, streamliner_options stream_opts_)
        : rules(s_rules)
        , stream_opts(stream_opts_)
        , foundations_base(card::rank_t(1))
        , stock(255)
        , waste(255)
        , hole (255) {
    // If there is a hole, creates pile
    if (rules.hole) {
        piles.emplace_back();
        hole = static_cast<pile::ref>(piles.size() - 1);
    }

    // If there are foundation piles, creates the relevant pile vectors
    if (rules.foundations_present) {
        for (uint8_t i = 0; i < 4*(rules.two_decks ? 2:1); i++) {
            piles.emplace_back();
            foundations.push_back(static_cast<pile::ref>(piles.size() - 1));
        }
    }

    // If there are cell piles, creates the relevant cell vectors
    if (rules.cells > 0) {
        for (uint8_t i = 0; i < rules.cells; i++) {
            piles.emplace_back();
            cells.push_back(static_cast<pile::ref>(piles.size() - 1));
            original_cells.push_back(static_cast<pile::ref>(piles.size() - 1));
        }
    }

    // If there is a stock, creates a pile
    if (rules.stock_size > 0) {
        piles.emplace_back();
        stock = static_cast<pile::ref>(piles.size() - 1);

        // If the stock deals to a waste, create the waste
        if (rules.stock_deal_t == sdt::WASTE) {
            piles.emplace_back();
            waste = static_cast<pile::ref>(piles.size() - 1);
        }
    }

    // If there is a reserve, creates piles
    if (rules.reserve_size > 0) {
        uint8_t pile_count = rules.reserve_stacked ?
                             uint8_t(1) : rules.reserve_size;
        for (uint8_t i = 0; i < pile_count; i++) {
            piles.emplace_back();
            reserve.push_back(static_cast<pile::ref>(piles.size() - 1));
            original_reserve.push_back(static_cast<pile::ref>(piles.size() - 1));
        }
    }

    // If there is a reserve, creates piles
    if (rules.accordion_size > 0) {
        uint8_t pile_count = rules.accordion_size;
        for (uint8_t i = 0; i < pile_count; i++) {
            piles.emplace_back();
            accordion.push_back(static_cast<pile::ref>(piles.size() - 1));
        }
    }

    // Creates the tableau piles
    for (uint8_t i = 0; i < rules.tableau_pile_count; i++) {
        piles.emplace_back();
        tableau_piles.push_back(static_cast<pile::ref>(piles.size() - 1));
        original_tableau_piles.push_back(static_cast<pile::ref>(piles.size() - 1));
    }

    // Creates the sequence piles
    for (uint8_t i = 0; i < rules.sequence_count; i++) {
        piles.emplace_back();
        sequences.push_back(static_cast<pile::ref>(piles.size() - 1));
    }
}

// Constructs an initial game state from a JSON doc
game_state::game_state(const sol_rules& s_rules, const Document& doc, streamliner_options s_opts)
        : game_state(s_rules, s_opts) {
    deal_parser::parse(*this, doc);
}

// Constructs an initial game state from a seed
game_state::game_state(const sol_rules& s_rules, int seed, streamliner_options s_opts)
        : game_state(s_rules, s_opts) {
    auto rng = mt19937(seed);
    vector<card> deck = gen_shuffled_deck(rules.max_rank, rules.two_decks, rng);

    // If there is a hole, moves the ace of spades to it
    if (rules.hole) {
        deck.erase(find(begin(deck), end(deck), card("AS")));
        place_card(hole, card("AS"));
    }


    // If the foundation base is random, picks the first card in the shuffled deck to be the base
    card::suit_t rand_suit = 0;
    if (!rules.foundations_base) { 
	card base_card = deck.front();
        foundations_base = base_card.get_rank();
        rand_suit = base_card.get_suit();
    }

    // If the foundations begin filled, then fills them
    uint8_t founds_to_fill = 0;
    switch (rules.foundations_init_cards) {
        case sol_rules::foundations_init_type::NONE:
            break;
        case sol_rules::foundations_init_type::ONE:
            founds_to_fill = 1; break;
        case sol_rules::foundations_init_type::ALL:
            founds_to_fill = static_cast<uint8_t>(4 * (rules.two_decks ? 2 : 1)); break;
    }
    for (uint8_t f_idx = 0; f_idx < founds_to_fill; f_idx++) {
        card c = card((f_idx + rand_suit) % uint8_t(4), foundations_base);

        deck.erase(find(begin(deck), end(deck), c));
        place_card(foundations[(f_idx + rand_suit)], c);
    }

    // If there are pre-filled cells, deals to them
    auto it = begin(original_cells);
    for (uint8_t i = 0; i < rules.cells_pre_filled; i++) {
        pile::ref r = *it;
        place_card(r, deck.back());
        deck.pop_back();
        it++;
    }

    // If there is a stock, deals to it and set up a waste pile too
    if (rules.stock_size > 0) {
        for (unsigned int i = 0; i < rules.stock_size; i++) {
            place_card(stock, deck.back());
            deck.pop_back();
        }
    }

    // If there is a reserve, deals to it.
    // We treat a regular reserve like multiple single-card piles,
    // but a stacked reserve as a single multiple-card pile.
    if (rules.reserve_size > 0) {
        for (unsigned int i = 0; i < rules.reserve_size; i++) {
            int idx = rules.reserve_stacked ? 0 : i;
            place_card(original_reserve[idx], deck.back());
            deck.pop_back();
        }
    }

    // Deals to the sequence piles
    if (rules.sequence_count > 0) {
        for (int t = 0; !deck.empty(); t++) {
            // Adds the randomly generated card to the tableau piles
            auto s = t % sequences.size();

            card c = deck.back();
            if (c.get_rank() == 1) c = "AS"; // The dummy "gap" placeholder

            pile::ref seq_pile = sequences[s];
            place_card(seq_pile, c);
            deck.pop_back();
        }
    }

    // Deals to the accordion
    if (rules.accordion_size > 0) {
        for (auto pr : accordion) {
            place_card(pr, deck.back());
            deck.pop_back();
        }
    }

    // This only occurs during testing
    if (rules.tableau_pile_count == 0) return;

    // Deals to the tableau piles (row-by-row)
    for (int t = 0; !deck.empty(); t++) {
        card c = deck.back();

        // If only the top cards are face up, initially deals all face down
        if (rules.face_up == fu::TOP_CARDS) c.turn_face_down();

        // Adds the randomly generated card to the tableau piles
        auto p = t % original_tableau_piles.size();

        // If we are doing a diagonal deal, each row should have one fewer card.
        // Leftover cards are dealt normally in full rows.
        auto row_idx = t / original_tableau_piles.size();
        if (rules.diagonal_deal && row_idx < original_tableau_piles.size()) {
            p = original_tableau_piles.size()-p-1;
            pile::ref tableau_pile = original_tableau_piles[p];

            if (p >= row_idx) {
                place_card(tableau_pile, c);
                deck.pop_back();
            }
        } else {
            pile::ref tableau_pile = original_tableau_piles[p];
            place_card(tableau_pile, c);
            deck.pop_back();
        }
    }

    // Now if necessary, turns the top cards face up
    if (rules.face_up == fu::TOP_CARDS)
        for (auto& p : piles)
            if (!p.empty())
                p[0].turn_face_up();

    // The size of all piles must equal the deck size
    int piles_sz = 0;
    for (auto& p : piles) piles_sz += p.size();
    if (piles_sz != rules.max_rank * (rules.two_decks ? 8:4)) {
        throw runtime_error("Error: incorrect number of cards in starting piles");
    }
}

game_state::game_state(const sol_rules& s_rules,
                       std::initializer_list<std::initializer_list<string>> il)
        : game_state(s_rules, sos::NONE) {pile::ref pr = 0;
    for (auto& p_il : il) {
        for (const string& card_str : p_il) {
            card c(card_str.c_str(), s_rules.face_up == fu::TOP_CARDS);
            place_card(pr, c);
        }
        pr++;
    }

    // Sets foundations base appropriately
    if (rules.foundations_present && rules.foundations_base == boost::none) {
        for (auto f : foundations) {
            if (!piles[f].empty()) {
                foundations_base = piles[f].top_card().get_rank();
                return;
            }
        }
    }
}

// Generates a randomly ordered vector of cards
vector<card> game_state::gen_shuffled_deck(card::rank_t max_rank,
                                           bool two_decks, mt19937 rng) {
    vector<card> deck;

    for (int deck_count = 1; deck_count <= (two_decks ? 2 : 1); deck_count++) {
        for (card::rank_t rank = 1; rank <= max_rank; rank++) {
            for (card::suit_t suit = 0 ; suit < 4; suit++) {
                deck.emplace_back(suit, rank);
            }
        }
    }

    assert(deck.size() == pile::size_type(max_rank * (two_decks ? 8 : 4)));

    game_state::shuffle(begin(deck), end(deck), rng);
    return deck;
}

template<class RandomIt, class URBG>
void game_state::shuffle(RandomIt first, RandomIt last, URBG&& g) {
    typedef typename std::iterator_traits<RandomIt>::difference_type diff_t;
    typedef boost::random::uniform_int_distribution<diff_t> distr_t;
    typedef typename distr_t::param_type param_t;

    distr_t D;
    diff_t n = last - first;
    for (diff_t i = n-1; i > 0; --i) {
        using std::swap;
        swap(first[i], first[D(g, param_t(0, i))]);
    }
}



////////////////////
// ALTERING STATE //
////////////////////

void game_state::make_move(const move m) {
    switch (m.type) {
        case move::mtype::regular:
	    make_regular_move(m); 
            break;
        case move::mtype::built_group:
            make_built_group_move(m);
            break;
        case move::mtype::stock_k_plus:
            make_stock_k_plus_move(m);
            break;
        case move::mtype::stock_to_all_tableau:
            make_stock_to_all_tableau_move(m);
            break;
        case move::mtype::sequence:
            make_sequence_move(m);
            break;
        case move::mtype::accordion:
            make_accordion_move(m);
            break;
        case move::mtype::null:
            assert(false);
            break;
    }

#ifndef NDEBUG
    check_face_down_consistent();
#endif
}

void game_state::undo_move(const move m) {
    switch (m.type) {
        case move::mtype::regular:
	    undo_regular_move(m); 
            break;
        case move::mtype::built_group:
            undo_built_group_move(m);
            break;
        case move::mtype::stock_k_plus:
            undo_stock_k_plus_move(m);
            break;
        case move::mtype::stock_to_all_tableau:
            undo_stock_to_all_tableau_move(m);
            break;
        case move::mtype::sequence:
            undo_sequence_move(m);
            break;
        case move::mtype::accordion:
            undo_accordion_move(m);
            break;
        case move::mtype::null:
            assert(false);
            break;
    }

#ifndef NDEBUG
    check_face_down_consistent();
#endif
}

void game_state::make_regular_move(const move m) {
    assert(m.from < piles.size());
    assert(m.to   < piles.size());

    place_card(m.to, take_card(m.from));

    if (m.reveal_move) {
        assert(!piles[m.from].empty());
        assert(piles[m.from][0].is_face_down());
        piles[m.from][0].turn_face_up();
    }

}

void game_state::undo_regular_move(const move m) {
    assert(m.to < piles.size());

    if (m.reveal_move) {
        assert(!piles[m.from].empty());
        assert(!piles[m.from][0].is_face_down());
        piles[m.from][0].turn_face_down();
    }

    place_card(m.from, take_card(m.to));

}

void game_state::make_built_group_move(move m) {
    assert(m.from  <  piles.size()  );
    assert(m.to    <  piles.size()  );

    // Adds the cards to the 'to' pile
    for (auto pile_idx = m.count; pile_idx-- > 0;) {
        place_card(m.to, piles[m.from][pile_idx]);
    }

    // Removes the cards from the 'from' pile
    for (uint8_t rem_count = 0; rem_count < m.count; rem_count++) {
        take_card(m.from);
    }

    if (m.reveal_move) {
        assert(!piles[m.from].empty());
        assert(piles[m.from][0].is_face_down());
        piles[m.from][0].turn_face_up();
    }
}

void game_state::undo_built_group_move(move m) {
    assert(m.to < piles.size());

    if (m.reveal_move) {
        assert(!piles[m.from].empty());
        assert(!piles[m.from][0].is_face_down());
        piles[m.from][0].turn_face_down();
    }

    // Adds the cards to the 'from' pile
    for (auto pile_idx = m.count; pile_idx-- > 0;) {
        place_card(m.from, piles[m.to][pile_idx]);
    }

    // Removes the cards from the 'to' pile
    for (uint8_t rem_count = 0; rem_count < m.count; rem_count++) {
        take_card(m.to);
    }
}

void game_state::make_stock_k_plus_move(const move m) {
#ifndef NDEBUG
    assert(rules.stock_deal_t == sdt::WASTE);
    assert(m.from == stock);
    if (rules.stock_redeal) assert(m.count <= piles[stock].size() && m.count > -piles[waste].size());
    assert(!rules.stock_redeal || m.to != waste);
    auto sz_before = piles[stock].size() + piles[waste].size();
#endif

    // Transfers count cards from the stock to the waste
    if (m.count > 0) {
        for (int i = 0; i < m.count; i++) {
            place_card(waste, take_card(stock));
        }
    } else {
        for (int i = 0; i > m.count; i--) {
            place_card(stock, take_card(waste));
        }
    }

    // Moves the card on top of the waste to the target pile
    place_card(m.to,  take_card(waste));

    // Flips the waste back on to the empty stock (if necessary)
    if (m.flip_waste) {
        assert(rules.stock_redeal && piles[stock].empty());
        while (!piles[waste].empty()) {
            place_card(stock, take_card(waste));
        }
    }


#ifndef NDEBUG
    auto sz_after = piles[stock].size() + piles[waste].size();
    assert(sz_before == sz_after + 1);
    assert(!(rules.stock_size > 0 && rules.stock_redeal && piles[stock].empty() && !piles[waste].empty()));
    assert(piles[stock].size() <= rules.stock_size);
#endif
}

void game_state::undo_stock_k_plus_move(move m) {
#ifndef NDEBUG
    assert(rules.stock_deal_t == sdt::WASTE);
    assert(m.from == stock);
    assert(!rules.stock_redeal || m.to != waste);
    auto sz_after = piles[stock].size() + piles[waste].size();
#endif

    if (m.flip_waste) {
        assert(rules.stock_redeal && piles[waste].empty());
        while (!piles[stock].empty()) {
            place_card(waste, take_card(stock));
        }
    }

    place_card(waste,  take_card(m.to));

    if (m.count > 0) {
        for (int i = 0; i < m.count; i++) {
            place_card(stock, take_card(waste));
        }
    } else {
        for (int i = 0; i > m.count; i--) {
            place_card(waste, take_card(stock));
        }
    }

#ifndef NDEBUG
    auto sz_before = piles[stock].size() + piles[waste].size();
    assert(sz_before == sz_after + 1);
    if (rules.stock_redeal) assert(m.count <= piles[stock].size() && m.count > -piles[waste].size());
    assert(!(rules.stock_size > 0 && rules.stock_redeal && piles[stock].empty() && !piles[waste].empty()));
    assert(piles[stock].size() <= rules.stock_size);
#endif
}

void game_state::make_stock_to_all_tableau_move(move m) {
    assert(rules.stock_deal_t == sdt::TABLEAU_PILES);

    for (pile::ref tab_pr = original_tableau_piles.front();
         tab_pr < pile::ref(original_tableau_piles.front() + m.count);
         tab_pr++) {
        place_card(tab_pr, take_card(stock));
    }
}

void game_state::undo_stock_to_all_tableau_move(move m) {
    assert(rules.stock_deal_t == sdt::TABLEAU_PILES);

    for (pile::ref tab_pr = original_tableau_piles.front() + m.count;
         tab_pr-- > original_tableau_piles.front();
            ) {
        place_card(stock, take_card(tab_pr));
    }
}

void game_state::make_sequence_move(const move m) {
    pile::ref from_seq_ref = m.from / rules.max_rank;
    pile::size_type from_card_idx = m.from % rules.max_rank;
    card from_card = piles[from_seq_ref][from_card_idx];
    piles[from_seq_ref][from_card_idx] = "AS";

    pile::ref to_seq_ref = m.to / rules.max_rank;
    pile::size_type to_card_idx = m.to % rules.max_rank;
    assert(piles[to_seq_ref][to_card_idx] == "AS");
    piles[to_seq_ref][to_card_idx] = from_card;
}

void game_state::undo_sequence_move(const move m) {
    pile::ref to_seq_ref = m.to / rules.max_rank;
    pile::size_type to_card_idx = m.to % rules.max_rank;
    card to_card = piles[to_seq_ref][to_card_idx];
    piles[to_seq_ref][to_card_idx] = "AS";

    pile::ref from_seq_ref = m.from / rules.max_rank;
    pile::size_type from_card_idx = m.from % rules.max_rank;
    assert(piles[from_seq_ref][from_card_idx] == "AS");
    piles[from_seq_ref][from_card_idx] = to_card;
}

void game_state::make_accordion_move(move m) {
    make_built_group_move(m);
    accordion.remove(m.from);
}

void game_state::undo_accordion_move(move m) {
    accordion.insert(upper_bound(begin(accordion), end(accordion), m.from), m.from);
    undo_built_group_move(m);
}

// Places a card on a pile and if it is on a tableau, cell or reserve pile,
// reorders the pile refs so that the largest pile is first
void game_state::place_card(pile::ref pr, card c) {
    piles[pr].place(c);

#ifndef NO_PILE_SYMMETRY
    // If the stock deals to the tableau piles, there is no pile symmetry
    if (rules.stock_size == 0 || rules.stock_deal_t != sdt::TABLEAU_PILES) {
        eval_pile_order(pr, true);
    }
#endif
}

// Same as above but for taking cards
card game_state::take_card(pile::ref pr) {
    card c = piles[pr].take();
#ifndef NO_PILE_SYMMETRY
    // If the stock deals to the tableau piles, there is no pile symmetry
    if (rules.stock_size == 0 || rules.stock_deal_t != sdt::TABLEAU_PILES) {
        eval_pile_order(pr, false);
    }
#endif
    return c;
}

#ifndef NDEBUG
void game_state::check_face_down_consistent() const {
    for (auto& p : original_tableau_piles) {
        if (piles[p].empty()) continue;
        // Makes sure the top cards of each tableau pile are face up
        assert(!piles[p].top_card().is_face_down());

        // Makes sure face down cards are never above face down ones
        bool seen_face_up = false;
        for (auto& c : piles[p].pile_vec) {
            seen_face_up = seen_face_up || !c.is_face_down();
            assert(!(c.is_face_down() && seen_face_up));
        }
    }
}
#endif


////////////////////////
// INSPECT GAME STATE //
////////////////////////

bool game_state::is_solved() const {
    bool solved = true;
    if (rules.hole) {
        solved = piles[hole].size()
                 == rules.max_rank * 4 * (rules.two_decks ? 2 : 1);
    } else if (rules.foundations_present) {
        for (auto f : foundations) {
            if (piles[f].size() != rules.max_rank) {
                solved = false;
            }
        }
    } else if (rules.sequence_count > 0) {
        for (pile::ref i = 0; i < sequences.size() && solved; i++) {
            for (pile::ref j = piles[sequences[i]].size(); j-- > 2;) {
                if (!is_next_legal_card(rules.sequence_build_pol, piles[sequences[i]][j-1], piles[sequences[i]][j])) {
                    solved = false;
                    break;
                }
            }
            if (piles[sequences[i]].top_card() != "AS") solved = false;
        }
    } else if (rules.accordion_size > 0) {
        return accordion.size() == 1;
    } else {
        assert(false);
    }

    // Runs some alternative checks in debug mode to make sure the game state
    // is consistent
#ifndef NDEBUG
    bool rest_empty = true;
    for (pile::ref pr = 0; pr < piles.size(); pr++) {
        // All piles other than the hole must be empty
        if (rules.hole) {
            if (pr != hole && !piles[pr].empty()) {
                rest_empty = false;
                break;
            }
        }
            // All piles other than the foundations must be empty
        else if (rules.foundations_present) {
            bool non_foundation_ref =
                    pr < foundations.front()
                    || pr >= foundations.front() + foundations.size();
            if (non_foundation_ref && !piles[pr].empty()) {
                rest_empty = false;
                break;
            }
        }
    }
    if (rules.sequence_count > 0) {
        for (auto seq : sequences) {
            assert(piles[seq].size() == rules.max_rank);
        }
    } else {
        assert(solved == rest_empty);
    }
#endif

    return solved;
}

const std::vector<pile>& game_state::get_data() const {
    return piles;
}


///////////
// PRINT //
///////////

ostream& operator<< (ostream& str, const game_state& gs) {
    return state_printer::print(str, gs);
}

