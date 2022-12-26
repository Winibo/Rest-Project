#ifndef CARD_H_INCLUDED
#define CARD_H_INCLUDED

#include <iostream>

/**
* @class Card
* @brief A card in a card game.
*/
class Card {

  public:
    /**
    * Suits of a standard card deck.
    */
    enum Suit {CLUB,DIAMOND,SPADE,HEART};

    /**
    * Ranks of a standard card deck.
    */
    enum Rank {ACE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE,TEN,JACK,QUEEN,KING};

    /**
    * A playing card.
    * @param s The suit of the card.
    * @param r The rank of the card.
    */
    Card(Suit s, Rank r):suit(s), rank(r) {}

    /**
    * @return A string representation of the card's rank.
    */
    static std::string getRank(Card::Rank r);

    /**
    * @return A string representation of the card's suit.
    */
    static std::string getSuit(Card::Suit s);

    /**
    * Suit of the card.
    */
    const Suit suit;

    /**
    * Rank of the card.
    */
    const Rank rank;
};


#endif // CARD_H_INCLUDED
