//
// Created by thecharlesblake on 10/11/17.
//

#ifndef SOLITAIRE_PIP_CARD_H
#define SOLITAIRE_PIP_CARD_H

#include <string>
#include <map>

class card {
public:
    // Types & typedefs
    typedef uint8_t rank_t;
    typedef uint8_t suit_t;
    typedef uint8_t colour_t;

    struct suit {
        static const suit_t Clubs;
        static const suit_t Hearts;
        static const suit_t Spades;
        static const suit_t Diamonds;
    };

    struct colour {
        static const colour_t Black;
        static const colour_t Red;
    };

    const static card divider;

    // A card can either be created using two int values (rank & suit), or
    // using a string (e.g. "AS")
    card(suit_t, rank_t, bool face_down=false);
    card(const char*, bool face_down_possible=false);
    card();

    suit_t get_suit() const;
    colour_t get_colour() const;
    rank_t get_rank() const;

    bool is_face_up() const;
    void turn_face_up();
    void turn_face_down();

    std::string to_string() const;

private:
    static suit_t suit_from_str(const char* c);
    static rank_t rank_from_str(const char* c);

    rank_t card_rank : 4;
    suit_t card_suit : 2;
    bool face_down : 1;
};

bool operator==(const card&, const card&);
bool operator!=(const card&, const card&);
bool operator<(const card&, const card&);
bool operator>(const card&, const card&);
bool operator<=(const card&, const card&);
bool operator>=(const card&, const card&);


#endif //SOLITAIRE_PIP_CARD_H
