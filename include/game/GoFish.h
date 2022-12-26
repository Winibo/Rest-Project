#ifndef GOFISH_H
#define GOFISH_H

#include <Game.h>

/**
* @class GoFish
* @brief Implementation of the Go Fish game.
*/
class GoFish : public Game {
  public:
    /**
    * Create a game of Go Fish with a deck of cards.
    * @param d The deck of cards for the game.
    */
    GoFish(Deck* d) : Game(d) {}
    virtual ~GoFish() {}

    /**
    * Deals cards to players. If there are < 4 players, each player gets 7 cards, otherwise each player gets 5 cards.
    * @param players The players in the game.
    */
    void dealCards(vector<Player*> players);

    /**
    * Checks if the hand contains a set of four cards of the same rank.
    * @param hand The hand to check.
    * @return true if a set was found, false otherwise. If a set was found, remove the cards from the hand.
    */
    static bool hasSet(list<Card*>* hand);

  private:
    const unsigned int NUM_CARDS_SMALL = 7;
    const unsigned int NUM_CARDS_LARGE = 5;

};

#endif // GOFISH_H
