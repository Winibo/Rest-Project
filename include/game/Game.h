#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <vector>
#include "Card.h"
#include "Deck.h"
#include "Player.h"

/**
* @class Game
* @brief A card game.
*/
class Game {
  public:
    /**
    * Create a game with a deck of cards.
    * @param d The deck of cards for the game.
    */
    Game(Deck* d):deck(d) {};

    virtual ~Game();

    /**
    * Deal the cards to players.
    * @param players The players of the game.
    */
    virtual void dealCards(vector<Player*> players) = 0;

    /**
    * Check if the game is over.
    * @return True if a player has no more cards in their hand, false otherwise.
    */
    virtual bool isOver() const;

    /**
    * Add a player to the game.
    * @param player The player to add.
    */
    void addPlayer(Player* player);

    /**
    * Get the players in the game.
    * @return A collection of players.
    */
    vector<Player*> getPlayers();

    /**
    * Start a game.
    * @throws game_init_error If there are no players in the game.
    */
    void start();

  protected:
    /**
    * Players of the game.
    */
    vector<Player*> players;

    /**
    * Card deck of the game.
    */
    Deck* deck;
};

#endif // GAME_H_INCLUDED
