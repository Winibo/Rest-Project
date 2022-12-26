/**
 * Copyright 2022 Dr. Anvik (john.anvik@uleth.ca), [INSERT YOUR NAME]
 */

#include <restbed>
#include <memory>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <exception>
#include <utility>
#include <nlohmann/json.hpp>
#include "gtest/gtest.h"
#include "rest/rest_utils.h"
#include "game/Card.h"
#include "rest/Server_Calls.h"

TEST(TestServer, NullGameChecks) {
  std::cout << "Sleeping" << std::endl;
  sleep(2);  //Sleep so the server has time to start
  std::cout << "Starting tests" << std::endl;
  //Game not created yet:
  //Ask if game is over
  std::shared_ptr<restbed::Request> request = createRequest("game/over");
  request->set_method("GET");
  auto response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  //Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  // Get players name
  request = createRequest("game/player/name");  // Endpoint to call
  request->set_header("Number", "0");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  //Draw a card
  request = createRequest("deck/draw");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  //Get Player 1 hand
  request = createRequest("game/player/hand");
  request->set_method("GET");
  request->set_header("Player", "0");
  std::cout << "Getting Hand" << std::endl;
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  //Shuffle Deck
  request = createRequest("deck/shuffle");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  //Add a card
  request = createRequest("game/player/add/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "3");
  request->set_header("Rank", "5");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  //Remove a card
  request = createRequest("game/player/remove/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "3");
  request->set_header("Rank", "5");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  //Check for set
  request = createRequest("game/player/set");
  request->set_header("Player", "0");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  //Get Score
  endpoint = "game/player/score";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  //Add a Point
  endpoint = "game/player/add/score";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  //Ask player for a card
  endpoint = "game/player/card";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_header("rank", "5");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
}

TEST(TestServer, getPlayerNameSuccess) {
  // Create a new game
  // Endpoint to call
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);
  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);

  // Get players name
  request = createRequest("game/player/name");  // Endpoint to call
  request->set_header("Number", "0");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json name = nlohmann::json::parse(result);
  EXPECT_EQ("Pat", name);  // Change name to whatever name you added
}

TEST(TestServer, getPlayerNameFail) {
  // Create a new game
  // Endpoint to call
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);
  // Add a player incorrectly
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  // Get invalid players name
  request = createRequest("game/player/name");  // Endpoint to call
  request->set_header("Number", "0");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  // Change name to whatever name you added
  EXPECT_EQ(response->get_status_code(), 400);
}

TEST(TestServer, addCardSuccess) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);

  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);

  // Add a player
  request = createRequest(endpoint);
  request->set_header("Name", "Joe");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  status = response->get_status_code();
  EXPECT_EQ(status, 200);
  //Add a card
  request = createRequest("game/player/add/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "3");
  request->set_header("Rank", "5");
  request->set_method("POST");
  restbed::Http::sync(request);
  //Get the player hand
  request = createRequest("game/player/hand");
  request->set_method("GET");
  request->set_header("Player", "0");
  std::cout << "Getting Hand" << std::endl;
  response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json hand = nlohmann::json::parse(result);
  EXPECT_EQ(hand.size(), 1);
  EXPECT_EQ(hand[0]["rank"], 5);
  EXPECT_EQ(hand[0]["suit"], 3);
  //For the Purposes of debugging by hand:
  std::cout << "Hand contains cards:" << std::endl;
  for (int i = 0; i < hand.size(); i++) {
    std::cout << "Card " << i + 1 << " is: " << Card::getRank(hand[i]["rank"])
              << " of " << Card::getSuit(hand[i]["suit"]) << std::endl;
  }
}

TEST(TestServer, addCardFailure) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);

  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);

  // Add a player
  request = createRequest(endpoint);
  request->set_header("Name", "Joe");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  status = response->get_status_code();
  EXPECT_EQ(status, 200);
  //Add a card
  request = createRequest("game/player/add/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "3");
  request->set_header("Rank", "5");
  request->set_method("POST");
  restbed::Http::sync(request);
  //Get the player hand
  request = createRequest("game/player/hand");
  request->set_method("GET");
  request->set_header("Player", "0");
  std::cout << "Getting Hand" << std::endl;
  response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json hand = nlohmann::json::parse(result);
  EXPECT_EQ(hand.size(), 1);
  EXPECT_EQ(hand[0]["rank"], 5);
  EXPECT_EQ(hand[0]["suit"], 3);
  //For the Purposes of debugging by hand:
  std::cout << "Hand contains cards:" << std::endl;
  for (int i = 0; i < hand.size(); i++) {
    std::cout << "Card " << i + 1 << " is: " << Card::getRank(hand[i]["rank"])
              << " of " << Card::getSuit(hand[i]["suit"]) << std::endl;
  }
  //Add a card again
  request = createRequest("game/player/add/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "3");
  request->set_header("Rank", "5");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  int status_code = response->get_status_code();
  EXPECT_EQ(400, status_code);
  //Add a card to an invalid player
  request = createRequest("game/player/add/card");
  request->set_header("Player", "10");
  request->set_header("Suit", "3");
  request->set_header("Rank", "5");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  status_code = response->get_status_code();
  EXPECT_EQ(400, status_code);
  //Get the player hand, again
  request = createRequest("game/player/hand");
  request->set_method("GET");
  request->set_header("Player", "0");
  std::cout << "Getting Hand" << std::endl;
  response = restbed::Http::sync(request);
  result = handle_response(response);
  hand = nlohmann::json::parse(result);
  EXPECT_EQ(hand.size(), 1);
  EXPECT_EQ(hand[0]["rank"], 5);
  EXPECT_EQ(hand[0]["suit"], 3);
  //For the Purposes of debugging by hand:
  std::cout << "Hand contains cards:" << std::endl;
  for (int i = 0; i < hand.size(); i++) {
    std::cout << "Card " << i + 1 << " is: " << Card::getRank(hand[i]["rank"])
              << " of " << Card::getSuit(hand[i]["suit"]) << std::endl;
  }
}

TEST(TestServer, removeCardSuccess) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);

  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);

  // Add a player
  request = createRequest(endpoint);
  request->set_header("Name", "Joe");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  status = response->get_status_code();
  EXPECT_EQ(status, 200);
  //Add a card
  request = createRequest("game/player/add/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "3");
  request->set_header("Rank", "5");
  request->set_method("POST");
  restbed::Http::sync(request);
  //Remove a card
  request = createRequest("game/player/remove/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "3");
  request->set_header("Rank", "5");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  int status_code = response->get_status_code();
  EXPECT_EQ(200, status_code);
  //Get the player hand
  request = createRequest("game/player/hand");
  request->set_method("GET");
  request->set_header("Player", "0");
  std::cout << "Getting Hand" << std::endl;
  response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json hand = nlohmann::json::parse(result);
  //For the Purposes of debugging by hand:
  std::cout << "Hand contains cards:" << std::endl;
  for (int i = 0; i < hand.size(); i++) {
    std::cout << "Card " << i + 1 << " is: " << Card::getRank(hand[i]["rank"])
              << " of " << Card::getSuit(hand[i]["suit"]) << std::endl;
  }
  EXPECT_EQ(hand.size(), 0);
}

TEST(TestServer, removeCardFailure) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);

  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);

  // Add a player
  request = createRequest(endpoint);
  request->set_header("Name", "Joe");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  status = response->get_status_code();
  EXPECT_EQ(status, 200);

  //Remove a card
  request = createRequest("game/player/remove/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "3");
  request->set_header("Rank", "5");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  int status_code = response->get_status_code();
  EXPECT_EQ(200, status_code);

  //Remove a card from an invalid player
  request = createRequest("game/player/remove/card");
  request->set_header("Player", "10");
  request->set_header("Suit", "3");
  request->set_header("Rank", "5");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  status_code = response->get_status_code();
  EXPECT_EQ(400, status_code);
  //Get the player hand
  request = createRequest("game/player/hand");
  request->set_method("GET");
  request->set_header("Player", "0");
  std::cout << "Getting Hand" << std::endl;
  response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json hand = nlohmann::json::parse(result);
  //For the Purposes of debugging by hand:
  std::cout << "Hand contains cards:" << std::endl;
  for (int i = 0; i < hand.size(); i++) {
    std::cout << "Card " << i + 1 << " is: " << Card::getRank(hand[i]["rank"])
              << " of " << Card::getSuit(hand[i]["suit"]) << std::endl;
  }
  EXPECT_EQ(hand.size(), 0);
}

TEST(TestServer, handIllegalPlayer) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);
  //Start Game
  request = createRequest("game/start");
  request->set_method("POST");
  restbed::Http::sync(request);
  //Get Player 1 hand
  request = createRequest("game/player/hand");
  request->set_method("GET");
  request->set_header("Player", "0");
  std::cout << "Getting Hand" << std::endl;
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(400, status);
}

TEST(TestServer, askPlayerSucceed) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);

  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);
  //Add a card
  request = createRequest("game/player/add/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "3");
  request->set_header("Rank", "5");
  request->set_method("POST");
  restbed::Http::sync(request);
  //Ask player for a card
  endpoint = "game/player/card";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_header("rank", "5");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  nlohmann::json answer = nlohmann::json::parse(handle_response(response));
  EXPECT_EQ(answer.size(), 1);
  //Add a card
  request = createRequest("game/player/add/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "2");
  request->set_header("Rank", "5");
  request->set_method("POST");
  restbed::Http::sync(request);
  //Ask player for a card
  endpoint = "game/player/card";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_header("rank", "5");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  answer = nlohmann::json::parse(handle_response(response));
  EXPECT_EQ(answer.size(), 2);
  //Add a card
  request = createRequest("game/player/add/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "1");
  request->set_header("Rank", "5");
  request->set_method("POST");
  restbed::Http::sync(request);
  //Ask player for a card
  endpoint = "game/player/card";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_header("rank", "5");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  answer = nlohmann::json::parse(handle_response(response));
  EXPECT_EQ(answer.size(), 3);
  //Add a card
  request = createRequest("game/player/add/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "0");
  request->set_header("Rank", "5");
  request->set_method("POST");
  restbed::Http::sync(request);
  //Ask player for a card
  endpoint = "game/player/card";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_header("rank", "5");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  answer = nlohmann::json::parse(handle_response(response));
  EXPECT_EQ(answer.size(), 4);
}

TEST(TestServer, askPlayerFail) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);

  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);

  //Ask player for a card
  endpoint = "game/player/card";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_header("rank", "0");
  request->set_header("suit", "0");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  nlohmann::json answer = nlohmann::json::parse(handle_response(response));
  EXPECT_EQ(answer.size(), 0);
  //Ask invalid player for a card
  endpoint = "game/player/card";
  request = createRequest(endpoint);
  request->set_header("Player", "10");
  request->set_header("rank", "0");
  request->set_header("suit", "0");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
}

TEST(TestServer, hasSet) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);

  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);;

  //Add a set:
  request = createRequest("game/player/add/card");
  request->set_header("Player", "0");
  request->set_header("Suit", "0");
  request->set_header("Rank", "5");
  request->set_method("POST");
  restbed::Http::sync(request);
  request->set_header("Suit", "1");
  restbed::Http::sync(request);
  request->set_header("Suit", "2");
  restbed::Http::sync(request);
  request->set_header("Suit", "3");
  restbed::Http::sync(request);

  //Check for set
  request = createRequest("game/player/set");
  request->set_header("Player", "0");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json set = nlohmann::json::parse(result);
  EXPECT_EQ(set, true);

  //Try again (should be false)
  request = createRequest("game/player/set");
  request->set_header("Player", "0");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  result = handle_response(response);
  set = nlohmann::json::parse(result);
  EXPECT_EQ(set, false);
  //Try again on invalid player
  request = createRequest("game/player/set");
  request->set_header("Player", "10");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
}

TEST(TestServer, gameCreate) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status_code = response->get_status_code();
  EXPECT_EQ(200, status_code);
}

TEST(TestServer, gameStart) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);

  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);

  // Add a player
  request = createRequest(endpoint);
  request->set_header("Name", "Joe");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  status = response->get_status_code();
  EXPECT_EQ(status, 200);
  //Start Game
  request = createRequest("game/start");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  int status_code = response->get_status_code();
  EXPECT_EQ(200, status_code);
}

TEST(TestServer, gameOver) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);

  //Ask if game is over
  request = createRequest("game/over");
  request->set_method("GET");
  auto response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json gameOver = nlohmann::json::parse(result);
  EXPECT_EQ(gameOver, false);  //All 0 players have at least 1 card

  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);

  //Ask if game is over
  request = createRequest("game/over");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  result = handle_response(response);
  gameOver = nlohmann::json::parse(result);
  EXPECT_EQ(gameOver, true);  //Player has 5 cards (hasn't played yet)

  //Start Game
  request = createRequest("game/start");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  int status_code = response->get_status_code();
  EXPECT_EQ(200, status_code);
  //Ask if game is over
  request = createRequest("game/over");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  result = handle_response(response);
  gameOver = nlohmann::json::parse(result);
  EXPECT_EQ(gameOver, false);  //Player has 7 cards (hasn't played yet)
}

TEST(TestServer, drawCard) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status_code = response->get_status_code();
  EXPECT_EQ(200, status_code);
  //Draw a card
  request = createRequest("deck/draw");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json jresult = nlohmann::json::parse(result);
  std::cout << jresult << std::endl;
  Card::Suit s = jresult["suit"];
  Card::Rank r = jresult["rank"];
  Card c(s, r);
  std::cout << c.suit << std::endl;
  std::cout << c.rank << std::endl;
  EXPECT_EQ(c.suit, Card::Suit(0));
  EXPECT_EQ(c.rank, Card::Rank(0));
  //Draw a card
  request = createRequest("deck/draw");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  result = handle_response(response);
  jresult = nlohmann::json::parse(result);
  std::cout << jresult << std::endl;
  s = jresult["suit"];
  r = jresult["rank"];
  Card c2(s, r);
  std::cout << c2.suit << std::endl;
  std::cout << c2.rank << std::endl;
  EXPECT_EQ(c2.suit, Card::Suit(0));
  EXPECT_EQ(c2.rank, Card::Rank(1));
}

TEST(TestServer, getHandTest) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);

  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);

  // Add a player
  request = createRequest(endpoint);
  request->set_header("Name", "Joe");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  status = response->get_status_code();
  EXPECT_EQ(status, 200);
  //Start Game
  request = createRequest("game/start");
  request->set_method("POST");
  restbed::Http::sync(request);
  //Get Player 1 hand
  request = createRequest("game/player/hand");
  request->set_method("GET");
  request->set_header("Player", "0");
  std::cout << "Getting Hand" << std::endl;
  response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json hand = nlohmann::json::parse(result);
  EXPECT_EQ(hand.size(), 7);
  //For the Purposes of debugging by hand:
  std::cout << "Hand contains cards:" << std::endl;
  for (int i = 0; i < hand.size(); i++) {
    std::cout << "Card " << i + 1 << " is: " << Card::getRank(hand[i]["rank"])
              << " of " << Card::getSuit(hand[i]["suit"]) << std::endl;
  }
}

TEST(TestServer, shuffleTest) {
  //Does not check anything (How do I check something randomly changed?)
  //Outputs results for manual debugging
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status_code = response->get_status_code();
  EXPECT_EQ(200, status_code);

  request = createRequest("deck/shuffle");
  request->set_method("POST");
  restbed::Http::sync(request);

  //Draw a card
  request = createRequest("deck/draw");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json jresult = nlohmann::json::parse(result);
  std::cout << jresult << std::endl;
  Card::Suit s = jresult["suit"];
  Card::Rank r = jresult["rank"];
  std::cout << "Card is: " << Card::getRank(r) << " of " << Card::getSuit(s)
            << std::endl;
  //Draw a card
  request = createRequest("deck/draw");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  result = handle_response(response);
  jresult = nlohmann::json::parse(result);
  std::cout << jresult << std::endl;
  s = jresult["suit"];
  r = jresult["rank"];
  std::cout << "Card is: " << Card::getRank(r) << " of " << Card::getSuit(s)
            << std::endl;
}

TEST(TestServer, addPointTest) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);

  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);

  //Add a Point
  endpoint = "game/player/add/score";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  nlohmann::json score = nlohmann::json::parse(handle_response(response));
  EXPECT_EQ(score, 1);
  //Add a second Point
  endpoint = "game/player/add/score";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  score = nlohmann::json::parse(handle_response(response));
  EXPECT_EQ(score, 2);
  //Add a point to an invalid player
  endpoint = "game/player/add/score";
  request = createRequest(endpoint);
  request->set_header("Player", "10");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
}

TEST(TestServer, getPointsTest) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");
  request->set_method("POST");
  restbed::Http::sync(request);

  // Add a player
  std::string endpoint = "game/player/add";  // Endpoint to call
  request = createRequest(endpoint);
  request->set_header("Name", "Pat");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  EXPECT_EQ(status, 200);

  //Get Score
  endpoint = "game/player/score";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  nlohmann::json score = nlohmann::json::parse(handle_response(response));
  EXPECT_EQ(score, 0);
  //Get Score from an invalid player
  endpoint = "game/player/score";
  request = createRequest(endpoint);
  request->set_header("Player", "10");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  EXPECT_EQ(response->get_status_code(), 400);
  //Add a Point
  endpoint = "game/player/add/score";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_method("POST");
  response = restbed::Http::sync(request);
  score = nlohmann::json::parse(handle_response(response));
  std::cout << "Score is: " << score << std::endl;
  EXPECT_EQ(score, 1);
  //Get Score
  endpoint = "game/player/score";
  request = createRequest(endpoint);
  request->set_header("Player", "0");
  request->set_method("GET");
  response = restbed::Http::sync(request);
  score = nlohmann::json::parse(handle_response(response));
  EXPECT_EQ(score, 1);
}

/**
 * A pseudo-test to shutdown server when testing is complete
 * Needed for computing code coverage of tests
 */
TEST(TestServer, shutdown) {
  std::shared_ptr<restbed::Request> request = createRequest("shutdown");
  restbed::Http::sync(request);
}
