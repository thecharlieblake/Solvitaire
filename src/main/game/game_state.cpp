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
#include "../input-output/output/log_helper.h"

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
game_state::game_state(const sol_rules& s_rules) : rules(s_rules),
                                                   stock(255),
                                                   waste(255),
                                                   hole(255) {
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
            auto_foundation_moves.push_back(true);
        }
    }

    // If there are cell piles, creates the relevant cell vectors
    if (rules.cells > 0) {
        for (uint8_t i = 0; i < rules.cells; i++) {
            piles.emplace_back();
            cells.push_back(static_cast<pile_ref>(piles.size() - 1));
            original_cells.push_back(static_cast<pile_ref>(piles.size() - 1));
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
            original_reserve.push_back(static_cast<pile_ref>(piles.size() - 1));
        }
    }

    // Creates the tableau piles
    for (uint8_t i = 0; i < rules.tableau_pile_count; i++) {
        piles.emplace_back();
        tableau_piles.push_back(static_cast<pile_ref>(piles.size() - 1));
        original_tableau_piles.push_back(static_cast<pile_ref>(piles.size() - 1));
    }
}

// Constructs an initial game state from a JSON doc
game_state::game_state(const sol_rules& s_rules, const Document& doc)
        : game_state(s_rules) {
    deal_parser::parse(*this, doc);
}

// Constructs an initial game state from a seed
game_state::game_state(const sol_rules& s_rules, int seed)
        : game_state(s_rules) {
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

        // Add the randomly generated card to the tableau piles
        auto p = t % original_tableau_piles.size();

        // If we are doing a diagonal deal, each row should have one fewer card.
        // Leftover cards are dealt normally in full rows.
        auto row_idx = t / original_tableau_piles.size();
        if (rules.diagonal_deal && row_idx < original_tableau_piles.size()) {
            p = original_tableau_piles.size()-p-1;
            pile_ref tableau_pile = original_tableau_piles[p];

            if (p >= row_idx) {
                place_card(tableau_pile, c);
                deck.pop_back();
            }
        } else {
            pile_ref tableau_pile = original_tableau_piles[p];
            place_card(tableau_pile, c);
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

game_state::game_state(const sol_rules& s_rules,
                       std::initializer_list<pile> il)
        : game_state(s_rules) {
    pile_ref pr = 0;
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
    auto rng = default_random_engine(seed+1);
    shuffle(begin(v_ptrs), end(v_ptrs), rng);

    vector<card> deck;
    for (uint8_t *i : v_ptrs) {
        auto r = static_cast<card::rank_t>(((*i) % max_rank) + 1);
        card::suit_t s = (*i) / max_rank;
        deck.emplace_back(card(s, r));
    }

    assert(deck.size() == pile::size_type(max_rank * (two_decks ? 8 : 4)));
    return deck;
}

const pile::size_type game_state::move::dominance_flag = pile::max_size_type;

game_state::move::move(pile_ref f, pile_ref t, pile::size_type i)
        : from(f), to(t), count(i) {
    assert(i >= 1);
}

// Returns true is the move has been flagged as a special 'dominance' move
bool game_state::move::is_dominance() const {
    return count == dominance_flag;
}


/////////////////////////
// MODIFYING FUNCTIONS //
/////////////////////////

void game_state::make_move(const move m) {
    // Handles special stock-to-tableau-piles move
    if (rules.stock_deal_t == sdt::TABLEAU_PILES && m.from == stock) {
        assert(m.from  == stock);
        assert(m.to    == 255  );

        for (pile_ref tab_pr = original_tableau_piles.front();
             tab_pr < pile_ref(original_tableau_piles.front() + m.count);
             tab_pr++) {
            place_card(tab_pr, take_card(stock));
        }
    }
    // If this is not a built-pile move
    else if (m.count == 1 || m.is_dominance()) {
        assert(m.from < piles.size());
        assert(m.to   < piles.size());

        place_card(m.to, take_card(m.from));
#ifndef NO_AUTO_FOUNDATIONS
        update_auto_foundation_moves(m.to);
#endif
    }
    // If this is a built-pile move
    else {
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
}

void game_state::undo_move(const move m) {
    assert(m.from < piles.size());

    // Handles special stock-to-tableau-piles move
    if (rules.stock_deal_t == sdt::TABLEAU_PILES && m.from == stock) {
        for (pile_ref tab_pr = original_tableau_piles.front() + m.count;
             tab_pr-- > original_tableau_piles.front();
                ) {
            place_card(stock, take_card(tab_pr));
        }
    }
    // If this is not a built-pile move
    else if (m.count == 1 || m.is_dominance()) {
        assert(m.to < piles.size());
        place_card(m.from, take_card(m.to));
#ifndef NO_AUTO_FOUNDATIONS
        update_auto_foundation_moves(m.from);
#endif
    } else {
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
}

// Places a card on a pile and if it is on a tableau, cell or reserve pile,
// reorders the pile refs so that the largest pile is first
void game_state::place_card(pile_ref pr, card c) {
    piles[pr].place(c);
#ifndef NO_PILE_SYMMETRY
    // If the stock deals to the tableau piles, there is no pile symmetry
    if (rules.stock_size == 0 || rules.stock_deal_t != sdt::TABLEAU_PILES) {
        eval_pile_order(pr, true);
    }
#endif
}

// Same as above but for taking cards
card game_state::take_card(pile_ref pr) {
    card c = piles[pr].take();
#ifndef NO_PILE_SYMMETRY
    // If the stock deals to the tableau piles, there is no pile symmetry
    if (rules.stock_size == 0 || rules.stock_deal_t != sdt::TABLEAU_PILES) {
        eval_pile_order(pr, false);
    }
#endif
    return c;
}

// Assesses whether the pile ref that was modified was a tableau, cell or
// reserve pile, and if so makes the relevant function call
void game_state::eval_pile_order(pile_ref pr, bool is_place) {
    if (!original_tableau_piles.empty()
        && pr >= original_tableau_piles[0]
        && pr <= original_tableau_piles[0] + original_tableau_piles.size()) {

        eval_pile_order(tableau_piles, pr, is_place);
    } else if (!original_cells.empty()
               && pr >= original_cells[0]
               && pr <= original_cells[0] + original_cells.size()) {

        eval_pile_order(cells, pr, is_place);
    } else if (!original_reserve.empty()
               && pr >= original_reserve[0]
               && pr <= original_reserve[0] + original_reserve.size()) {

        eval_pile_order(reserve, pr, is_place);
    }
}

// Finds the pile ref in the list and evaluates whether it should be moved to
// maintain the "pile order"
void game_state::eval_pile_order(list<pile_ref>& pile_lst, pile_ref changed_pr,
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

// Decides which 'auto foundation' booleans should be 'true' currently
void game_state::update_auto_foundation_moves(pile_ref target_pile) {
    // If there are no foundations, or this is not a foundation move, do nothing
    if (!rules.foundations
        || target_pile < foundations.front()
        || target_pile > foundations.back()) {
        return;
    }

    // Turns the 'auto' boolean for the target foundation false

    for (pile_ref pr : foundations) {
        auto_foundation_moves[pr] = is_valid_auto_foundation_move(pr);
    }
}

bool game_state::is_valid_auto_foundation_move(pile_ref target_pile) const {
    if (rules.build_pol == pol::ANY_SUIT || rules.build_pol == pol::RED_BLACK) {
        card::suit_t target_suit(target_pile - foundations.front());
        card::rank_t target_rank = piles[target_pile].empty() ? card::rank_t(0)
                                   : piles[target_pile].top_card().get_rank();
        card target_card = card(target_suit, target_rank);

        for (pile_ref pr : foundations) {
            if (pr == target_pile) continue;

            card::suit_t foundation_suit(pr - foundations.front());
            card::rank_t foundation_rank = piles[pr].empty() ? card::rank_t(0)
                                       : piles[pr].top_card().get_rank();
            card foundation_card = card(foundation_suit, foundation_rank);
            int rank_diff = int(target_rank)
                            - int(foundation_card.get_rank());

            if (rules.build_pol == pol::RED_BLACK
                && foundation_card.get_colour() == target_card.get_colour()) {
                if (rank_diff >= 3) {
                    return false;
                }
            } else if (rank_diff >= 2) {
                return false;
            }
        }
    }
    return true;
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

const std::vector<pile>& game_state::get_data() const {
    return piles;
}


///////////
// PRINT //
///////////

ostream& operator<< (ostream& str, const game_state& gs) {
    return state_printer::print(str, gs);
}
