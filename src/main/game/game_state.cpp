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
#include "../input-output/deal_parser.h"

using namespace std;
using namespace rapidjson;
using namespace boost;

typedef sol_rules::build_order ord;
typedef sol_rules::build_policy pol;
typedef sol_rules::spaces_policy s_pol;

// A private constructor used by both of the public ones. Initializes all of the
// piles and pile refs specified by the rules
game_state::game_state(const sol_rules& s_rules) :
        rules(s_rules) {

    // Creates the tableau piles
    for (uint8_t i = 0; i < rules.tableau_pile_count; i++) {
        piles.push_back(pile::tableau_factory(rules.build_ord, rules.build_pol,
                                              rules.spaces_pol));
        tableau_piles.push_back(static_cast<pile_ref>(piles.size() - 1));
    }

    // If there are foundation piles, creates the relevant pile vectors
    if (rules.foundations) {
        for (uint8_t i = 0; i < 4; i++) {
            piles.push_back(pile::foundation_factory(pol(i)));
            foundations.push_back(static_cast<pile_ref>(piles.size() - 1));
        }
    }

    // If there are cell piles, creates the relevant cell vectors
    if (rules.cells > 0) {
        for (uint8_t i = 0; i < rules.cells; i++) {
            piles.push_back(pile::cell_factory());
            cells.push_back(static_cast<pile_ref>(piles.size() - 1));
        }
    }

    // If there is a reserve, creates a reserve pile
    if (rules.reserve_size > 0) {
        piles.push_back(pile::reserve_factory());
        reserve = static_cast<pile_ref>(piles.size() - 1);
    }

    // If there is a stock & waste, creates piles
    if (rules.stock_size > 0) {
        piles.push_back(pile::stock_factory());
        stock = static_cast<pile_ref>(piles.size() - 1);
        piles.push_back(pile::waste_factory());
        waste = static_cast<pile_ref>(piles.size() - 1);
    }

    if (rules.hole) {
        piles.push_back(pile::hole_factory(s_rules.max_rank));
        hole = static_cast<pile_ref>(piles.size() - 1);
    }

    for (pile_ref pr = 0; pr < piles.size(); pr++) {
        if (rules.stock_size > 0 && pr == waste) continue;

        if (piles[pr].can_remove()) {
            removable_piles.emplace_back(pr);
        }
        if (piles[pr].get_build_order() != ord::NO_BUILD) {
            addable_piles.emplace_back(pr);
        }
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

    // If there is a reserve, deals to it
    if (rules.reserve_size > 0) {
        for (unsigned int i = 0; i < rules.reserve_size; i++) {
            piles[reserve].place(deck.back());
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

void game_state::make_move(const move m) {
    piles[m.second].place(piles[m.first].take());
}

void game_state::undo_move(const move m) {
    piles[m.first].place(piles[m.second].take());
}

vector<game_state::move> game_state::get_legal_moves() const {
    // The next legal states
    vector<move> moves;

    // Dealing from the stock to the waste
    if (rules.stock_size > 0 && !piles[stock].empty()) {
        moves.emplace_back(stock, waste);
    }

    // Moving from and to the 'can remove' and 'can add' piles
    // Note traversal order. Solver evaluates backwards, so we reverse traversal
    // order of removable piles, but not addable. This moves from tableau piles
    // to foundations first if possible
    for (auto rem_it = removable_piles.rbegin(); rem_it != removable_piles.rend(); ++rem_it) {
        if (piles[*rem_it].empty()) continue;

        for (const pile_ref add_pile : addable_piles) {
            if (add_pile != *rem_it &&
                    piles[add_pile].can_place(piles[*rem_it].top_card())) {
                moves.emplace_back(*rem_it, add_pile);
            }
        }
    }

    return moves;
}

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

ostream& game_state::print(ostream& stream) const {
    if (rules.foundations) {
        print_header(stream, "Foundations");
        print_top_of_piles(stream, foundations);
    }
    if (rules.cells) {
        print_header(stream, "Cells");
        print_piles(stream, cells);
    }
    if (rules.reserve_size > 0) {
        print_header(stream, "Reserve");
        print_pile(stream, reserve);
    }
    if (rules.tableau_pile_count > 0) {
        print_header(stream, "Tableau Piles");
        print_piles(stream, tableau_piles);
    }
    if (rules.stock_size > 0) {
        print_header(stream, "Stock | Waste");
        print_piles(stream, {stock, waste});
    }
    if (rules.hole) {
        print_header(stream, "Hole Card");
        print_top_of_pile(stream, hole);
    }
    return stream << "===================================";
}

void game_state::print_header(ostream& stream, const char* header) const {
    stream << "--- " << header << " ";
    size_t pad = 20 - strlen(header);
    for (size_t i = 0; i < pad; i++) {
        stream << '-';
    }
    stream << "\n";
}

void game_state::print_piles(ostream& stream, const vector<pile_ref>& pile_rs) const {
    bool empty_row = false;
    vector<card>::size_type row_idx = 0;

    // Loops through the rows in each pile, starting from the bottom, until
    // one is empty
    while (!empty_row) {
        // Loops through the current row to determine if it is empty
        empty_row = true;
        for (const pile_ref pile_r : pile_rs) {
            if (piles[pile_r].size() > row_idx) {
                empty_row = false;
                break;
            }
        }
        if (!empty_row || row_idx == 0) {
            // Loops through the current (non-empty) row, and outputs the values
            for (const pile_ref pile_r : pile_rs) {
                if (piles[pile_r].size() > row_idx) {
                    stream << piles[pile_r][row_idx];
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

void game_state::print_pile(ostream& stream, const pile_ref pile_r) const {
    print_piles(stream, {pile_r});
}

void game_state::print_top_of_piles(ostream& stream, const vector<pile_ref>& vp) const {
    vector<pile_ref> top(vp);

    for (pile_ref p : top) {
        if (piles[p].empty()) {
            stream << "[]";
        } else {
            stream << piles[p].top_card();
        }
        stream << "\t";
    }
    stream << "\n";
}

void game_state::print_top_of_pile(ostream& stream, const pile_ref pile_r) const {
    print_top_of_piles(stream, {pile_r});
}

bool operator==(const game_state& a, const game_state& b) {
    return a.tableau_piles == b.tableau_piles
            && a.foundations == b.foundations
            && a.hole == b.hole;
}

ostream& operator <<(ostream& stream, const game_state& gs) {
    return gs.print(stream);
}

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

    if (gs.rules.reserve_size > 0) {
        hash_combine(seed, gs.piles[gs.reserve]);
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
