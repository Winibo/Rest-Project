/*
 * Server_Calls.h
 *
 *  Created on: Dec. 4, 2022
 *      Author: Liam
 */

#ifndef INCLUDE_REST_SERVER_CALLS_H_
#define INCLUDE_REST_SERVER_CALLS_H_

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
#include "Card.h"

//Calls to the restbed server
//If server does not return a value, returns a boolean for if the call was valid
//If it does, returns a pair where the first value is if it was a call returned OK
//and the second is the return value of the call

bool createGame() {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("game/create");  // Endpoint to call
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  if (status == 200)
    return true;
  return false;  //Should never be reached (unless the server is off)
}

bool startGame() {
  //Start Game
  std::shared_ptr<restbed::Request> request = createRequest("game/start");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  if (status == 200)
    return true;
  return false;  //Should never be reached (unless server is off)
}

bool addPlayer(std::string name) {
  std::string endpoint = "game/player/add";  // Endpoint to call
  std::shared_ptr<restbed::Request> request = createRequest(endpoint);
  request->set_header("Name", name);
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  int status = response->get_status_code();
  if (status == 200)
    return true;
  return false;  //Should never be reached (unless server is off)
}

bool gameOver() {
  //Ask if game is over
  std::shared_ptr<restbed::Request> request = createRequest("game/over");
  request->set_method("GET");
  auto response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  bool gameover = nlohmann::json::parse(result).get<bool>();
  return gameover;
}

std::string getName(int player) {
  std::shared_ptr<restbed::Request> request = createRequest("game/player/name");  // Endpoint to call
  request->set_header("Number", std::to_string(player));
  request->set_method("GET");
  auto response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  std::string name = nlohmann::json::parse(result).get<std::string>();
  return name;
}

Card drawCard() {
  std::shared_ptr<restbed::Request> request = createRequest("deck/draw");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  nlohmann::json jresult = nlohmann::json::parse(handle_response(response));
  Card::Suit s = jresult["suit"];
  Card::Rank r = jresult["rank"];
  Card card(s, r);
  return card;
}

std::vector<Card> getHand(int player) {
  std::shared_ptr<restbed::Request> request = createRequest("game/player/hand");
  request->set_method("GET");
  request->set_header("Player", std::to_string(player));
  auto response = restbed::Http::sync(request);
  bool result = false;
  std::vector<Card> hand;
  if (response->get_status_code() == 200) {
    std::string result = handle_response(response);
    nlohmann::json jsonhand = nlohmann::json::parse(result);
    for (int i = 0; i < jsonhand.size(); i++) {
      Card c(Card::Suit(jsonhand[i]["suit"]), Card::Rank(jsonhand[i]["rank"]));
      hand.push_back(c);
    }
  }
  return hand;
}

bool shuffle() {
  std::shared_ptr<restbed::Request> request = createRequest("deck/shuffle");
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  bool result = false;
  if (response->get_status_code() == 200)
    result = true;
  return result;
}

bool addCard(int player, Card card) {
  std::shared_ptr<restbed::Request> request = createRequest(
      "game/player/add/card");
  request->set_header("Player", std::to_string(player));
  request->set_header("Suit", std::to_string(card.suit));
  request->set_header("Rank", std::to_string(card.rank));
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  if (response->get_status_code() == 200)
    return true;
  return false;
}

bool removeCard(int player, Card card) {
  std::shared_ptr<restbed::Request> request = createRequest(
      "game/player/remove/card");
  request->set_header("Player", std::to_string(player));
  request->set_header("Suit", std::to_string(card.suit));
  request->set_header("Rank", std::to_string(card.rank));
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  if (response->get_status_code() == 200)
    return true;
  return false;
}

bool hasSet(int player) {
  std::shared_ptr<restbed::Request> request = createRequest("game/player/set");
  request->set_header("Player", std::to_string(player));
  request->set_method("GET");
  auto response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json set = nlohmann::json::parse(result);
  return set;
}

int getScore(int player) {
  std::shared_ptr<restbed::Request> request = createRequest(
      "game/player/score");
  request->set_header("Player", std::to_string(player));
  request->set_method("GET");
  auto response = restbed::Http::sync(request);
  if (response->get_status_code() == 200) {
    int score = nlohmann::json::parse(handle_response(response)).get<int>();
    return score;
  }
  return 0;
}

int addScore(int player) {
  std::shared_ptr<restbed::Request> request = createRequest(
      "game/player/add/score");
  request->set_header("Player", std::to_string(player));
  request->set_method("POST");
  auto response = restbed::Http::sync(request);
  if (response->get_status_code() == 200) {
    int score = nlohmann::json::parse(handle_response(response)).get<int>();
    return score;
  }
  return 0;
}

std::vector<Card> hasCard(int player, int rank) {
  std::shared_ptr<restbed::Request> request = createRequest("game/player/card");
  request->set_header("Player", std::to_string(player));
  request->set_header("rank", std::to_string(rank));
  request->set_method("GET");
  auto response = restbed::Http::sync(request);
  std::vector<Card> cards;
  if (response->get_status_code() == 200) {
    nlohmann::json answer = nlohmann::json::parse(handle_response(response));
    for (int i = 0; i < answer.size(); i++) {
      Card::Suit s = answer[i]["suit"];
      Card::Rank r = answer[i]["rank"];
      Card c(s, r);
      cards.push_back(c);
    }
  } else {
    std::cout << "An error occurred asking if player " << player
              << " had cards of rank " << rank << std::endl;
  }
  return cards;
}

//Does NOT return a value, since it can't know if it succeeded (the server will be off if it worked)
void shutdown() {
  std::shared_ptr<restbed::Request> request = createRequest("shutdown");
  restbed::Http::sync(request);
}
#endif /* INCLUDE_REST_SERVER_CALLS_H_ */
