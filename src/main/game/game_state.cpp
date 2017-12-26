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

// Construct an initial game state from a JSON doc
game_state::game_state(const sol_rules& s_rules) :
        rules(s_rules),
        reserve(pile::reserve_factory()),
        stock(pile::stock_factory()),
        waste(pile::waste_factory()),
        hole(pile::hole_factory(s_rules.max_rank)) {
}

// Construct an initial game state from a JSON doc
game_state::game_state(const sol_rules& s_rules, const Document& doc) :
        game_state(s_rules) {
    deal_parser::parse(*this, doc);
}

// Construct an initial game state from a seed
game_state::game_state(const sol_rules& s_rules, int seed) :
        game_state(s_rules) {

    vector<card> deck = shuffled_deck(seed, rules.max_rank);

    // If there is a hole, move the ace of spades to it
    if (rules.hole) {
        deck.erase(find(begin(deck), end(deck), card("AS")));
        hole.place("AS");
    }

    // If there is a stock, deal to it and set up a waste pile too
    if (rules.stock_size > 0) {
        for (unsigned int i = 0; i < rules.stock_size; i++) {
            stock.place(deck.back());
            deck.pop_back();
        }
    }

    // If there is a reserve, deal to it
    if (rules.reserve_size > 0) {
        for (unsigned int i = 0; i < rules.reserve_size; i++) {
            reserve.place(deck.back());
            deck.pop_back();
        }
    }

    // Creates the tableau piles
    for (unsigned int i = 0; i < rules.tableau_pile_count; i++) {
        tableau_piles.push_back(pile::tableau_factory(rules.build_ord));
    }

    // Deal to the tableau piles (row-by-row)
    unsigned int t = 0;
    while (!deck.empty()) {
        card c = deck.back();
        deck.pop_back();

        // Add the randomly generated card to the tableau piles
        tableau_piles[t++ % rules.tableau_pile_count].place(c);
    }

    // If there are foundation piles, create the relevant pile vectors
    if (rules.foundations) {
        for (int i = 0; i < 4; i++) {
            foundations.push_back(pile::foundation_factory(pol(i)));
        }
    }

    // If there are cell piles, create the relevant cell vectors
    if (rules.cells > 0) {
        for (unsigned int i = 0; i < rules.cells; i++) {
            cells.push_back(pile::cell_factory());
        }
    }
}

// Generate a randomly ordered vector of cards
vector<card> game_state::shuffled_deck(int seed, int max_rank = 13) {
    vector<int> values;
    vector<int*> v_ptrs;
    for (int i = 0; i < max_rank * 4; i++) {
        values.push_back(i);
    }
    for (int i = 0; i < max_rank * 4; i++) {
        v_ptrs.push_back(&values[i]);
    }

    // Randomly shuffle the pointers
    auto rng = default_random_engine(seed);
    shuffle(begin(v_ptrs), end(v_ptrs), rng);

    vector<card> deck;
    for (int *i : v_ptrs) {
        int r = ((*i) % max_rank) + 1;
        int s = (*i) / max_rank;

        deck.emplace_back(card(r, s));
    }

    return deck;
}

const vector<game_state> game_state::get_next_legal_states() const {
    // Generates vectors of references to all the piles from which
    // cards can be removed, and to which they can be added
    vector<pile*> can_remove;
    vector<pile*> can_add;

    // A mutable version of the current state, used to create the next legal
    // states
    game_state mut_state = *this;

    for (pile* p : mut_state.get_regular_pile_refs()) {
        if (p->can_remove()) {
            can_remove.push_back(p);
        }
        if (p->get_build_order() != ord::NO_BUILD) {
            can_add.push_back(p);
        }
    }

    // The next legal states
    vector<game_state> next;

    // Moving from and to the 'can remove' and 'can add' piles
    for (pile* rem_pile : can_remove) {
        if (rem_pile->empty()) continue;

        for (pile* add_pile : can_add) {
            if (&add_pile != &rem_pile
                && add_pile->can_place(rem_pile->top_card())) {

                game_state new_gs = move(mut_state, rem_pile, add_pile);
                next.push_back(new_gs);
            }
        }
    }

    // Dealing from the stock to the waste
    if (!stock.empty()) {
        game_state new_gs = move(mut_state, &mut_state.stock, &mut_state.waste);
        next.push_back(new_gs);
    }

    return next;
}

const game_state game_state::move(game_state& gs, pile* from, pile* to) const {
    to->place(from->take());
    const game_state new_gs(gs);
    from->place(to->take());
    return new_gs;
}


bool game_state::is_solved() const {
    for (auto p : foundations) {
        if (p.size() != rules.max_rank) {
            return false;
        }
    }

    return !rules.hole || hole.size() == rules.max_rank * 4;
}

// Not the stock, because cards from it can't be moved to the rest of the piles!
vector<pile*> game_state::get_regular_pile_refs() {
    vector<pile*> piles;
    auto to_ptr = [](pile& p){return &p;};

    if (rules.foundations) {
        transform(begin(foundations), end(foundations), back_inserter(piles),
                  to_ptr);
    }
    if (rules.cells) {
        transform(begin(cells), end(cells), back_inserter(piles), to_ptr);
    }
    transform(begin(tableau_piles), end(tableau_piles), back_inserter(piles), to_ptr);
    if (rules.reserve_size > 0) {
        piles.push_back(&reserve);
    }
    if (rules.stock_size > 0) {
        piles.push_back(&waste);
    }
    if (rules.hole) {
        piles.push_back(&hole);
    }
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
    int pad = 20 - strlen(header);
    for (int i = 0; i < pad; i++) {
        stream << '-';
    }
    stream << "\n";
}

void game_state::print_piles(ostream& stream,
                             const vector<pile>& pile_vec) const {
    bool empty_row = false;
    vector<card>::size_type row_idx = 0;

    // Loops through the rows in each pile, starting from the bottom, until
    // one is empty
    while (!empty_row) {
        // Loops throught the current row to determine if it is empty
        empty_row = true;
        for (const pile& tableau_pile : pile_vec) {
            if (tableau_pile.size() > row_idx) {
                empty_row = false;
                break;
            }
        }
        if (!empty_row || row_idx == 0) {
            // Loops through the current (non-empty) row, and outputs the values
            for (const pile& tableau_pile : pile_vec) {
                if (tableau_pile.size() > row_idx) {
                    stream << tableau_pile[row_idx];
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

void game_state::print_pile(ostream& stream, const pile &p) const {
    print_piles(stream, {p});
}

void game_state::print_top_of_piles(ostream& stream, const vector<pile>& vp) const {
    vector<pile> top(vp);
    for (pile& p : top) {
        if (!p.empty()) {
            card t = p.top_card();
            p.clear();
            p.place(t);
        }
    }
    print_piles(stream, top);
}

void game_state::print_top_of_pile(ostream& stream, const pile& p) const {
    print_top_of_piles(stream, {p});
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
        hash_combine(seed, gs.reserve);
    }
    if (gs.rules.stock_size > 0) {
        hash_combine(seed, gs.stock);
        hash_combine(seed, gs.waste);
    }
    if (gs.rules.hole) {
        hash_combine(seed, gs.hole);
    }

    return seed;
}

size_t hash_value(vector<pile> const& vp) {
    return hash_range(begin(vp), end(vp));
}
