/**
 * Copyright 2022 Dr. Anvik (john.anvik@uleth.ca)
 */

#include "rest/GoFishUI_REST.h"

unsigned int GoFishUI_REST::choosePlayer(unsigned int playerNum,
                                         unsigned int numPlayers) {
  unsigned int selection = playerNum;
  std::cout << "You are Player #" << playerNum << std::endl;
  while (selection == playerNum || selection >= numPlayers) {
    std::cout << "Which player (0-" << numPlayers - 1 << ")? ";
    std::cin >> selection;
    if (selection == playerNum)
      std::cout << "That is you! Please choose another player." << std::endl;
    if (selection >= numPlayers)
      std::cout << "Player #" << selection
                << " doesn't exist! Please choose another player." << std::endl;
  }
  return selection;
}

unsigned int GoFishUI_REST::requestCard(std::vector<Card> hand) {
  std::cout << "Your hand:" << std::endl;
  unsigned int index = 0;
  for (Card c : hand) {
    Card::Rank r = c.rank;
    std::cout << "\tChoice " << (++index) << ": " << Card::getRank(r)
              << std::endl;
  }

  unsigned int selection = 0;
  while (selection == 0 || selection > hand.size()) {
    std::cout << "Select a choice: ";
    std::cin >> selection;
  }
  return selection - 1;
}

void GoFishUI_REST::playFailed() {
  std::cout << "Go Fish!" << std::endl;
}

void GoFishUI_REST::playSucceeded() {
  std::cout << "They had some of those cards!" << std::endl;
}

void GoFishUI_REST::showScores(std::vector<std::string> names,
                               std::vector<int> scores) {
  std::cout << "---- Scores ----" << std::endl;
  for (int i = 0; i < names.size(); i++) {
    std::cout << "Score for " << names[i] << ": " << scores[i] << std::endl;
  }
}
