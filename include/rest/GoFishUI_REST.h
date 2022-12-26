#ifndef GOFISH_REST_UI_H_INCLUDED
#define GOFISH_REST_UI_H_INCLUDED

/**
 * Copyright 2022 Dr. Anvik (john.anvik@uleth.ca)
 */

#include <iostream>
#include <nlohmann/json.hpp>
#include "Card.h"

class GoFishUI_REST {
 public:
  /**
   * Ask a user to choose a player to ask for a card
   * @param playerNum The id of the player to ask.
   * @param numPlayers The total number of players in the game.
   * @return The id of the player to ask.
   */
  unsigned int choosePlayer(unsigned int playerNum, unsigned int numPlayers);

  /**
   * Show the current player's hand and ask which rank to request.
   * @param hand The player's current hand in JSON format.
   * @return The rank of the card to ask for.
   */
  unsigned int requestCard(std::vector<Card> hand);

  /**
   * The player didn't have the requested card.
   */
  void playFailed();

  /**
   * The player had the requested card.
   */
  void playSucceeded();

  /**
   * Show the scores of the players.
   * @param players A JSON array of the players and their scores.
   * Expected format is [{name:<player name>,score:<score>}].
   */
  void showScores(std::vector<std::string> names, std::vector<int> scores);
};

#endif
