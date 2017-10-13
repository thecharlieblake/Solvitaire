#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include "Card.h"
#include <boost/log/trivial.hpp>

#if defined NDEBUG
    #define LOG_DEBUG(msg)
#else
    #define LOG_DEBUG(msg) BOOST_LOG_TRIVIAL(debug) << msg
#endif
#define LOG_INFO(msg) BOOST_LOG_TRIVIAL(info) << msg
#define LOG_WARNING(msg) BOOST_LOG_TRIVIAL(warning) << msg
#define LOG_ERROR(msg) BOOST_LOG_ERROR(error) << msg

using namespace std;

bool search(vector<vector<Card*>* >, Card* hole);

int main()
{
    const int topRank = 4;

    vector<vector<Card*>* > tableaux;

    vector<Card*> shuffleCards;
    for (int r = 1; r <= topRank; r++) {
        for (int s = 0; s < 4; s++) {
            string suit;
            if (s == 0) {
                suit = "D";
            } else if (s == 1) {
                suit = "S";
            } else if (s == 2) {
                suit = "H";
            } else {
                suit = "C";
            }

            Card* c = new Card(r, suit);
            shuffleCards.push_back(c);
        }
    }

    auto rng = std::default_random_engine(2); // 3 has solution
    std::shuffle(std::begin(shuffleCards), std::end(shuffleCards), rng);

    for (int i = 0; i < topRank * 2; i++) {
        vector<Card*>* tab = new vector<Card*>();
        for (int j = 0; j < 2; j++) {
            int idx = i + (topRank * 2 * j);
            if (shuffleCards[idx]->suit == "S" && shuffleCards[idx]->rank == 1) {
                continue;
            } else {
                tab->push_back(shuffleCards[idx]);
            }
        }
        tableaux.push_back(tab);
    }

    Card c(2, string("H"));
    LOG_DEBUG(c);

    cout << (search(tableaux, new Card(1, "S")) ? "Solution Found" : "No Solution");

    return 0;
}

bool search(vector<vector<Card*>* > tableaux, Card* hole) {
    // If the tableaux are all empty, return true

    // Seaches through each tableau. If the top card is one away from
    // the hole card, puts it in the hole and recurses.
    // If that fails then put the card back and repeat for the next foundation
    // If it succeeds, return true

    // If we get to the end of the function, return false

#ifndef NDEBUG
    string stateRep;
    for (unsigned int i = 0; i < 2; i++) {
        for (auto tableau : tableaux) {
            if (tableau->size() > i) {
                cout << *(*tableau)[i];
            }
            cout << "\t";
        }
        cout << "\n";
    }
    cout << "Hole: " << *hole << "\n";
#endif

    bool allTableauxEmpty = true;
    for (unsigned int i = 0; i < tableaux.size(); i++) {
        auto tableau = tableaux[i];

        if (tableau->empty()) {
            continue;
        } else {
            allTableauxEmpty = false;
        }

        Card* topCard = tableau->back();
        if (topCard->rank == hole->rank + 1 || topCard->rank == hole->rank - 1) {
            tableau->pop_back();

            bool success = search(tableaux, topCard);

            if (success) {
                return true;
            } else {
                tableau->push_back(topCard);
            }
        }
    }

    return allTableauxEmpty;
}