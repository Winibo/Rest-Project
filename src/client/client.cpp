/**
 * Copyright 2022 Dr. Anvik (john.anvik@uleth.ca)
 */

#include <restbed>
#include <memory>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>

#include "gtest/gtest.h"
#include "rest/rest_utils.h"
#include "rest/GoFishUI_REST.h"
#include "rest/Server_Calls.h"

#include "Game.h"

void askPlayer(unsigned int turnPlayer, unsigned int askPlayer,
               unsigned int askRank, GoFishUI_REST *ui) {
  // Ask player
  std::vector<Card> cards = hasCard(askPlayer, askRank);
  if (cards.size() > 0) {
    ui->playSucceeded();
    // Move card between players
    for (int i = 0; i < cards.size(); i++) {
      //Remove from player 1
      removeCard(askPlayer, cards[i]);
      //Add to player 2
      addCard(turnPlayer, cards[i]);
    }
    return;
  } else {
    ui->playFailed();
    // Draw card
    Card c = drawCard();
    addCard(turnPlayer, c);
    return;
  }
}

bool isGameOver() {
  return gameOver();
}

int main(const int, const char**) {
  std::string playerNames[] = { "Alice", "Bob" };

  GoFishUI_REST *ui = new GoFishUI_REST();

  // Start game
  createGame();
  for (std::string player : playerNames)
    addPlayer(player);
  startGame();
  // Play game
  unsigned int numPlayers = 2;
  unsigned int turn = 0;
  //But Before we begin, check a player didn't start with a set:
  for (int i = 0; i < numPlayers; i++) {
    if (hasSet(i)) {
      addScore(i);
      std::cout << getName(i) << ", you started with a set!" << std::endl;
    }
  }
  //Now we can begin
  while (!isGameOver()) {
    //Print who's turn it is:
    std::cout << getName(turn) << ", it is your turn." << std::endl;
    // Choose a player to ask
    int playerAsked = (turn + 1) % numPlayers;
    // Show current players hand and get asked rank
    vector<Card> hand = getHand(turn);
    int index = ui->requestCard(hand);

    // Ask the player
    unsigned int askedRank = hand[index].rank;

    askPlayer(turn, playerAsked, askedRank, ui);

    // Check for sets
    if (hasSet(turn)) {
      addScore(turn);
      std::cout << "You have a set!" << std::endl;
    }
    turn = ++turn % numPlayers;
  }

  // Show scores
  std::cout << "===== Game Over =====" << std::endl;
  vector<int> scores;
  vector<std::string> names;
  for (int i = 0; i < numPlayers; i++) {
    names.push_back(getName(i));
    scores.push_back(getScore(i));
  }
  ui->showScores(names, scores);

  delete ui;

  return EXIT_SUCCESS;
}
