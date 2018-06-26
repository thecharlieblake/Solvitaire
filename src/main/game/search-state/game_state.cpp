//
// Created by thecharlesblake on 10/24/17.
//

#include <vector>
#include <ostream>
#include <random>
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

typedef sol_rules::build_policy pol;
typedef sol_rules::stock_deal_type sdt;
typedef sol_rules::face_up_policy fu;

//////////////////
// CONSTRUCTORS //
//////////////////

// A private constructor used by both of the public ones. Initializes all of the
// piles and pile refs specified by the rules
game_state::game_state(const sol_rules& s_rules, bool streamliners_)
        : rules(s_rules)
        , streamliners(streamliners_)
        , stock(255)
        , waste(255)
        , hole (255) {
    // If there is a hole, creates pile
    if (rules.hole) {
        piles.emplace_back();
        hole = static_cast<pile::ref>(piles.size() - 1);
    }

    // If there are foundation piles, creates the relevant pile vectors
    if (rules.foundations) {
        for (uint8_t i = 0; i < 4*(rules.two_decks ? 2:1); i++) {
            piles.emplace_back();
            foundations.push_back(static_cast<pile::ref>(piles.size() - 1));
            auto_foundation_moves.push_back(true);
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

    // Creates the tableau piles
    for (uint8_t i = 0; i < rules.tableau_pile_count; i++) {
        piles.emplace_back();
        tableau_piles.push_back(static_cast<pile::ref>(piles.size() - 1));
        original_tableau_piles.push_back(static_cast<pile::ref>(piles.size() - 1));
    }
}

// Constructs an initial game state from a JSON doc
game_state::game_state(const sol_rules& s_rules, const Document& doc, bool streamliners_)
        : game_state(s_rules, streamliners_) {
    deal_parser::parse(*this, doc);
}

// Constructs an initial game state from a seed
game_state::game_state(const sol_rules& s_rules, int seed, bool streamliners_)
        : game_state(s_rules, streamliners_) {
    vector<card> deck = gen_shuffled_deck(seed, rules.max_rank, rules.two_decks);

    // If there is a hole, moves the ace of spades to it
    if (rules.hole) {
        deck.erase(find(begin(deck), end(deck), card("AS")));
        place_card(hole, card("AS"));
    }

    // If the foundations begin filled, then fills them
    if (rules.foundations_init_card) {
        for (uint8_t f_idx = 0; f_idx < 4*(rules.two_decks ? 2:1); f_idx++) {
            card c = card(f_idx % uint8_t(4), 1);

            deck.erase(find(begin(deck), end(deck), c));
            place_card(foundations[f_idx], c);
        }
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
                       std::initializer_list<pile> il)
        : game_state(s_rules, false) {
    pile::ref pr = 0;
    for (const pile& p : il) {
        for (const card c : p.pile_vec) {
            place_card(pr, c);
        }
        pr++;
    }
}

// Generates a randomly ordered vector of cards
vector<card> game_state::gen_shuffled_deck(int seed, card::rank_t max_rank,
                                           bool two_decks) {
    vector<card> deck;

    for (int deck_count = 1; deck_count <= (two_decks ? 2 : 1); deck_count++) {
        for (card::rank_t rank = 1; rank <= max_rank; rank++) {
            for (card::suit_t suit = 0 ; suit < 4; suit++) {
                deck.emplace_back(suit, rank);
            }
        }
    }

    assert(deck.size() == pile::size_type(max_rank * (two_decks ? 8 : 4)));

    auto rng = mt19937(seed);
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
        case move::mtype::dominance:
            make_regular_move(m);
            break;
        case move::mtype::built_group:
            make_built_group_move(m);
            break;
        case move::mtype::stock_to_waste:
            make_stock_to_waste_move(m);
            break;
        case move::mtype::stock_to_tableau:
            make_stock_to_tableau_move(m);
            break;
        case move::mtype::redeal:
            make_redeal_move();
            break;
        case move::mtype::null:
            assert(false);
            break;
    }
}

void game_state::undo_move(const move m) {
    switch (m.type) {
        case move::mtype::regular:
        case move::mtype::dominance:
            undo_regular_move(m);
            break;
        case move::mtype::built_group:
            undo_built_group_move(m);
            break;
        case move::mtype::stock_to_waste:
            undo_stock_to_waste_move(m);
            break;
        case move::mtype::stock_to_tableau:
            undo_stock_to_tableau_move(m);
            break;
        case move::mtype::redeal:
            undo_redeal_move();
            break;
        case move::mtype::null:
            assert(false);
            break;
    }
}

void game_state::make_regular_move(const move m) {
    assert(m.from < piles.size());
    assert(m.to   < piles.size());

    place_card(m.to, take_card(m.from));

#ifndef NO_AUTO_FOUNDATIONS
    update_auto_foundation_moves(m.to);
#endif
}

void game_state::undo_regular_move(const move m) {
    assert(m.to < piles.size());

    place_card(m.from, take_card(m.to));

#ifndef NO_AUTO_FOUNDATIONS
    update_auto_foundation_moves(m.from);
#endif
}

void game_state::make_built_group_move(move m) {
    assert(rules.move_built_group);
    assert(m.from  <  piles.size()  );
    assert(m.to    <  piles.size()  );
    assert(m.count <= rules.max_rank);

    // Adds the cards to the 'to' pile
    for (auto pile_idx = m.count; pile_idx-- > 0;) {
        place_card(m.to, piles[m.from][pile_idx]);
    }

    // Removes the cards from the 'from' pile
    for (uint8_t rem_count = 0; rem_count < m.count; rem_count++) {
        take_card(m.from);
    }
}

void game_state::undo_built_group_move(move m) {
    assert(rules.move_built_group);
    assert(m.to < piles.size());
    assert(m.count <= rules.max_rank);

    // Adds the cards to the 'from' pile
    for (auto pile_idx = m.count; pile_idx-- > 0;) {
        place_card(m.from, piles[m.to][pile_idx]);
    }

    // Removes the cards from the 'to' pile
    for (uint8_t rem_count = 0; rem_count < m.count; rem_count++) {
        take_card(m.to);
    }
}

void game_state::make_stock_to_waste_move(const move m) {
    assert(rules.stock_deal_t == sdt::WASTE);

    for (int i = 0; i < m.count; i++) {
        place_card(waste, take_card(stock));
    }
}

void game_state::undo_stock_to_waste_move(const move m) {
    assert(rules.stock_deal_t == sdt::WASTE);

    for (int i = 0; i < m.count; i++) {
        place_card(stock, take_card(waste));
    }
}

void game_state::make_stock_to_tableau_move(const move m) {
    assert(rules.stock_deal_t == sdt::TABLEAU_PILES);

    for (pile::ref tab_pr = original_tableau_piles.front();
         tab_pr < pile::ref(original_tableau_piles.front() + m.count);
         tab_pr++) {
        place_card(tab_pr, take_card(stock));
    }
}

void game_state::undo_stock_to_tableau_move(const move m) {
    assert(rules.stock_deal_t == sdt::TABLEAU_PILES);

    for (pile::ref tab_pr = original_tableau_piles.front() + m.count;
         tab_pr-- > original_tableau_piles.front();
            ) {
        place_card(stock, take_card(tab_pr));
    }
}

void game_state::make_redeal_move() {
    assert(piles[stock].empty());

    // Re-deal waste back to stock
    while (!piles[waste].empty()) {
        place_card(stock, take_card(waste));
    }
}

void game_state::undo_redeal_move() {
    assert(piles[waste].empty());

    while (!piles[stock].empty()) {
        place_card(waste, take_card(stock));
    }
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


////////////////////////
// INSPECT GAME STATE //
////////////////////////

bool game_state::is_solved() const {
    bool solved = true;
    if (rules.hole) {
        solved = piles[hole].size()
                 == rules.max_rank * 4 * (rules.two_decks ? 2 : 1);
    } else {
        assert(rules.foundations);
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
    for (pile::ref pr = 0; pr < piles.size(); pr++) {
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

const std::vector<pile>& game_state::get_data() const {
    return piles;
}


///////////
// PRINT //
///////////

ostream& operator<< (ostream& str, const game_state& gs) {
    return state_printer::print(str, gs);
}

