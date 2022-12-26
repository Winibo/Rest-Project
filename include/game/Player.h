#ifndef PLAYER_H
#define PLAYER_H

using namespace std;

#include <string>
#include <list>
#include "Card.h"

/**
* @class Player
* @brief A player in a card game.
*/
class Player {
  public:
    /**
    * A player in a card game.
    * @param n The name of the player.
    */
    Player(const string& n) : name(n) {}

    virtual ~Player();

    /**
    * Add a card to the player's hand.
    * @param c The card to add.
    */
    void addCard(Card* c);

    /**
    * Get a card from the player's hand.
    * @param index Index of the card in the player's hand (0-relative).
    */
    Card* getCard(unsigned int index);

    /**
    * Get the cards in the hand of the player.
    * @return The cards in player's hand.
    */
    list<Card*>* getHand();

    /**
    * Add points to a player's score.
    * @param points The amount of points to add.
    */
    void addPoints(int points);

    /**
    * Get the score of the player.
    * @return The player's score.
    */
    unsigned int getScore();

    /**
    * Name of the player.
    */
    const string name;

  private:
    list<Card*> hand;
    unsigned int score = 0;
};

#endif // PLAYER_H
