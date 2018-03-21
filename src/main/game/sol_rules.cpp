//
// Created by thecharlesblake on 1/12/18.
//

#include "sol_rules.h"

typedef sol_rules::build_policy pol;
typedef sol_rules::spaces_policy s_pol;
typedef sol_rules::stock_deal_type sdt;

// Note that the 'default' sol rules are set up by parsing the default json
// rules description. This initialisation is really just for testing purposes.
sol_rules::sol_rules() :
        tableau_pile_count(0),
        build_pol(pol::ANY_SUIT),
        spaces_pol(s_pol::ANY),
        move_built_group(false),
        built_group_pol(pol::ANY_SUIT),
        two_decks(false),
        max_rank(13),
        hole(false),
        foundations(false),
        foundations_init_card(false),
        foundations_comp_piles(false),
        diagonal_deal(false),
        cells(0),
        stock_size(0),
        stock_deal_t(sdt::TABLEAU_PILES),
        reserve_size(0),
        reserve_stacked(false) {
}
