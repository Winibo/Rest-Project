#ifndef DECK_H
#define DECK_H

using namespace std;

#include "Card.h"
#include <vector>

/**
* @class Deck
* @brief A deck of cards in a card game.
*/
class Deck {
  public:

    virtual ~Deck();
    /**
    * Shuffle the deck of cards.
    */
    virtual void shuffle();

    /**
    * Add a card to the deck.
    * @param card The card to add.
    */
    virtual void addCard(Card* card);

    /**
    * Get the top card from the deck.
    * @return The card drawn.
    */
    virtual Card* getCard();

    /**
    * Get the size of the deck.
    * @return The size of the deck.
    */
    virtual unsigned int size();

    /**
    * Create a standard deck where suits=[HEART, CLUB, DIAMOND, SPADE] and rank=[Ace, 2-9, Jack, Queen, King]
    */
    void createStandard();

  private:
    vector<Card*> deck;
};

#endif // DECK_H
