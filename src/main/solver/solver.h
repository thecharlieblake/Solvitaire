//
// Created by thecharlesblake on 10/24/17.
//

#ifndef SOLVITAIRE_SOLVER_H
#define SOLVITAIRE_SOLVER_H

#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include "game/game_state.h"
#include "game/sol_rules.h"

class solver {
public:
    typedef boost::multi_index::multi_index_container<
    game_state,
    boost::multi_index::indexed_by<
            boost::multi_index::sequenced<>,
            boost::multi_index::hashed_unique<boost::multi_index::identity<game_state>>
        >
    > node_history;

    struct node {
        node(const node*, const game_state&);
        node_history history;
        game_state state;

        static const node_history gen_history(const node* parent);
    };

    solver(const game_state, const sol_rules&);

    const node get_root() const;
    int get_states_searched() const;

    const boost::optional<node> run();

    friend std::ostream& operator<< (std::ostream&, const solver::node&);

private:

    const node root;
    const sol_rules rules;
    node solution;
    int states_searched;
};


#endif //SOLVITAIRE_SOLVER_H
