/**
 * Copyright 2022 Dr. Anvik (john.anvik@uleth.ca), [INSERT YOUR NAME]
 */

#include <restbed>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>

#include "Game.h"
#include "GoFish.h"
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "Exceptions.h"

// REST endpoint
static const char ENDPOINT[] = "/gofish";

const int PORT = 1234;  // Choose your own port number > 1024

// Response header to prevent a cross-site validation problem
#define ALLOW_ALL { "Access-Control-Allow-Origin", "*" }

// Response header to close connection
#define CLOSE_CONNECTION { "Connection", "close" }

Deck *deck = nullptr;
Game *game = nullptr;
restbed::Service service;

void shutdown_handler(const std::shared_ptr<restbed::Session> &session) {
  std::cout << "Shutting down server" << std::endl;
  service.stop();
  if (game != nullptr)
    delete game;
  exit(0);
}

void CreateGame(const std::shared_ptr<restbed::Session> &session) {
  if (game != nullptr) {
    delete game;
    game = nullptr;
    deck = nullptr;
  }
  deck = new Deck();
  deck->createStandard();
  game = new GoFish(deck);
  std::cout << std::endl << "Creating new game" << std::endl;
  session->close(restbed::OK);
}

void StartGame(const std::shared_ptr<restbed::Session> &session) {
  std::cout << "Starting game..." << std::endl;
  game->start();
  session->close(restbed::OK);
}

void IsGameOver(const std::shared_ptr<restbed::Session> &session) {
  if (game == nullptr) {
    session->close(restbed::BAD_REQUEST);
    return;
  }
  bool result = game->isOver();
  nlohmann::json resultJSON = result;
  std::string response = resultJSON.dump();
  // Send response
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

void addPlayer(const std::shared_ptr<restbed::Session> &session) {
  if (game == nullptr) {
    session->close(restbed::BAD_REQUEST);
    return;
  }
  const auto request = session->get_request();
  string name = request->get_header("Name", "");
  if (name == "") {
    session->close(restbed::BAD_REQUEST);
    return;
  }
  cout << "Adding Player" << endl;
  Player *player = new Player(name);
  game->addPlayer(player);
  session->close(restbed::OK);
}

void getName(const std::shared_ptr<restbed::Session> &session) {
  const auto request = session->get_request();
  size_t number = request->get_header("Number", 0);
  if (game == nullptr) {
    session->close(restbed::BAD_REQUEST);
    return;
  }
  vector<Player*> players = game->getPlayers();
  if (players.size() <= number) {
    session->close(restbed::BAD_REQUEST);
    return;
  }
  string name = players[number]->name;
  nlohmann::json resultJSON = name;
  std::string response = resultJSON.dump();
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

void drawCard(const std::shared_ptr<restbed::Session> &session) {
  if (deck == nullptr) {
    session->close(restbed::BAD_REQUEST);
    return;
  }
  Card *c;
  cout << "Drawing Card" << endl;
  c = deck->getCard();
  nlohmann::json result;
  result["suit"] = c->suit;
  std::cout << "Drew Suit:" << c->suit << std::endl;
  result["rank"] = c->rank;
  std::cout << "Drew Rank:" << c->rank << std::endl;
  std::string response = result.dump();
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

void getHand(const std::shared_ptr<restbed::Session> &session) {
  const auto request = session->get_request();
  size_t player = request->get_header("Player", 0);
  if (game == nullptr) {
    session->close(restbed::BAD_REQUEST);
    return;
  }
  std::vector<Player*> players = game->getPlayers();
  std::cout << "Getting Player " << player << "'s hand" << std::endl;
  std::cout << "There are " << players.size() << " players" << std::endl;
  if (players.size() <= player) {
    session->close(restbed::BAD_REQUEST);
    return;
  }
  list<Card*> hand = *players[static_cast<int>(player)]->getHand();
  cout << "Player " << player << " Has " << hand.size() << " Cards" << endl;
  nlohmann::json jarray;
  for (Card *c : hand) {
    cout << "Player " << player << " Has: ";
    cout << Card::getRank(c->rank) << " of " << Card::getSuit(c->suit) << endl;
    nlohmann::json card;
    card["rank"] = c->rank;
    card["suit"] = c->suit;
    jarray.push_back(card);
  }
  std::string response = jarray.dump();
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

void shuffleDeck(const std::shared_ptr<restbed::Session> &session) {
  if (deck == nullptr) {
    session->close(restbed::BAD_REQUEST);
    return;
  }
  deck->shuffle();
  session->close(restbed::OK);
}

void addCard(const std::shared_ptr<restbed::Session> &session) {
  if (game == nullptr) {
    session->close(restbed::BAD_REQUEST);  //No players to add cards to
    return;
  }
  const auto request = session->get_request();
  size_t player = request->get_header("Player", 0);
  vector<Player*> players = game->getPlayers();
  if (players.size() <= player) {
    session->close(restbed::BAD_REQUEST);  //Player doesn't exist
    return;
  }
  size_t suit = request->get_header("Suit", 0);
  size_t rank = request->get_header("Rank", 0);
  cout << "Adding Card: " << Card::getRank(Card::Rank(rank));
  cout << " of " << Card::getSuit(Card::Suit(suit)) << " to Player ";
  cout << player << "'s hand." << endl;
  Card *c = new Card(Card::Suit(suit), Card::Rank(rank));
  list<Card*> *hand = players[player]->getHand();
  // We only check if the player we are giving the card too already has it
  // Maybe we should check if anybody has that card?? - Liam
  auto checkDuplicates = [&c](Card *card) {
    return (card->rank == c->rank && card->suit == c->suit);
  };
  if (any_of(hand->begin(), hand->end(), checkDuplicates)) {
    delete c;
    session->close(restbed::BAD_REQUEST);
    return;
  }
  players[player]->addCard(c);
  session->close(restbed::OK);
}

void removeCard(const std::shared_ptr<restbed::Session> &session) {
  if (game == nullptr) {
    session->close(restbed::BAD_REQUEST);  //No players to add cards to
    return;
  }
  const auto request = session->get_request();
  size_t player = request->get_header("Player", 0);
  vector<Player*> players = game->getPlayers();
  if (players.size() <= player) {
    session->close(restbed::BAD_REQUEST);  //Player doesn't exist
    return;
  }
  size_t suit = request->get_header("Suit", 0);
  size_t rank = request->get_header("Rank", 0);
  cout << "Removing Card: " << Card::getRank(Card::Rank(rank));
  cout << " of " << Card::getSuit(Card::Suit(suit)) << " from Player ";
  cout << player << "'s hand." << endl;
  Card *c = new Card(Card::Suit(suit), Card::Rank(rank));
  list<Card*> *hand = players[player]->getHand();
  for (auto itr = hand->begin(); itr != hand->end(); itr++) {
    if ((*itr)->rank == c->rank && (*itr)->suit == c->suit) {
      //Player Already has card
      hand->erase(itr);  //Remove the card
      break;
    }
  }
  delete c;
  session->close(restbed::OK);
}

void hasSet(const std::shared_ptr<restbed::Session> &session) {
  if (game == nullptr) {
    session->close(restbed::BAD_REQUEST);  // No game
    return;
  }
  const auto request = session->get_request();
  size_t player = request->get_header("Player", 0);
  if (game->getPlayers().size() <= player) {
    session->close(restbed::BAD_REQUEST);  // Invalid Player
    return;
  }
  std::cout << "Checking for a set on player: " << player << std::endl;
  //Check if they have a set
  list<Card*> *hand = game->getPlayers()[player]->getHand();
  nlohmann::json result = GoFish::hasSet(hand);
  std::string response = result.dump();
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

void getScore(const std::shared_ptr<restbed::Session> &session) {
  if (game == nullptr) {
    session->close(restbed::BAD_REQUEST);  // No game
    return;
  }
  const auto request = session->get_request();
  size_t player = request->get_header("Player", 0);
  if (game->getPlayers().size() <= player) {
    session->close(restbed::BAD_REQUEST);  // Invalid Player
    return;
  }
  std::cout << "Getting score for player: " << player << std::endl;
  nlohmann::json result = game->getPlayers()[player]->getScore();
  std::string response = result.dump();
  std::cout << "Score is: " << response << std::endl;
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

void addPoint(const std::shared_ptr<restbed::Session> &session) {
  if (game == nullptr) {
    session->close(restbed::BAD_REQUEST);  // No game
    return;
  }
  const auto request = session->get_request();
  size_t player = request->get_header("Player", 0);
  if (game->getPlayers().size() <= player) {
    session->close(restbed::BAD_REQUEST);  // Invalid Player
    return;
  }
  std::cout << "Adding point to player " << player << "'s score" << std::endl;
  game->getPlayers()[player]->addPoints(1);
  getScore(session);  //Return the player's new score
}

void hasCard(const std::shared_ptr<restbed::Session> &session) {
  if (game == nullptr) {
    session->close(restbed::BAD_REQUEST);  // No game
    return;
  }
  const auto request = session->get_request();
  size_t player = request->get_header("Player", 0);
  if (game->getPlayers().size() <= player) {
    session->close(restbed::BAD_REQUEST);  // Invalid Player
    return;
  }
  size_t rank = request->get_header("rank", 0);
  std::cout << "Checking if Player " << player << " Has cards of rank "
            << Card::getRank(Card::Rank(rank)) << " in their hand" << endl;
  list<Card*> hand = *(game->getPlayers()[player]->getHand());
  cout << "Player's hand size is: " << hand.size() << endl;
  nlohmann::json jhand;
  for (Card *b : hand) {
    std::cout << "Checking Card: ";
    std::cout << Card::getRank(b->rank) << " of " << Card::getSuit(b->suit);
    std::cout << endl;
    if (b->rank == Card::Rank(rank)) {
      nlohmann::json jcard;
      jcard["rank"] = b->rank;
      jcard["suit"] = b->suit;
      jhand.push_back(jcard);
    }
  }
  std::string result = jhand.dump();
  session->close(restbed::OK, result, { ALLOW_ALL, { "Content-Length",
      std::to_string(result.length()) }, CLOSE_CONNECTION });
}

int main(const int, const char**) {
// Setup service and request handlers
  auto shutdown = std::make_shared<restbed::Resource>();
  shutdown->set_path("/shutdown");
  shutdown->set_method_handler("GET", shutdown_handler);

  auto settings = std::make_shared<restbed::Settings>();
  settings->set_port(PORT);
  settings->set_root(ENDPOINT);

  auto startgame = std::make_shared<restbed::Resource>();
  startgame->set_path("/game/start");
  startgame->set_method_handler("POST", StartGame);

  auto creategame = std::make_shared<restbed::Resource>();
  creategame->set_path("/game/create");
  creategame->set_method_handler("POST", CreateGame);

  auto isgameover = std::make_shared<restbed::Resource>();
  isgameover->set_path("/game/over");
  isgameover->set_method_handler("GET", IsGameOver);

  auto addplayer = std::make_shared<restbed::Resource>();
  addplayer->set_path("game/player/add");
  addplayer->set_method_handler("POST", addPlayer);

  auto playername = std::make_shared<restbed::Resource>();
  playername->set_path("game/player/name");
  playername->set_method_handler("GET", getName);

  auto drawcard = std::make_shared<restbed::Resource>();
  drawcard->set_path("/deck/draw");
  drawcard->set_method_handler("POST", drawCard);

  auto gethand = std::make_shared<restbed::Resource>();
  gethand->set_path("game/player/hand");
  gethand->set_method_handler("GET", getHand);

  auto shuffledeck = std::make_shared<restbed::Resource>();
  shuffledeck->set_path("/deck/shuffle");
  shuffledeck->set_method_handler("POST", shuffleDeck);

  auto addcard = std::make_shared<restbed::Resource>();
  addcard->set_path("game/player/add/card");
  addcard->set_method_handler("POST", addCard);

  auto removecard = std::make_shared<restbed::Resource>();
  removecard->set_path("game/player/remove/card");
  removecard->set_method_handler("POST", removeCard);

  auto hasset = std::make_shared<restbed::Resource>();
  hasset->set_path("game/player/set");
  hasset->set_method_handler("GET", hasSet);

  auto getscore = std::make_shared<restbed::Resource>();
  getscore->set_path("game/player/score");
  getscore->set_method_handler("GET", getScore);

  auto addscore = std::make_shared<restbed::Resource>();
  addscore->set_path("game/player/add/score");
  addscore->set_method_handler("POST", addPoint);

  auto hascard = std::make_shared<restbed::Resource>();
  hascard->set_path("game/player/card");
  hascard->set_method_handler("GET", hasCard);
  // Publish and start service
  service.publish(shutdown);
  service.publish(creategame);
  service.publish(startgame);
  service.publish(isgameover);
  service.publish(addplayer);
  service.publish(playername);
  service.publish(drawcard);
  service.publish(gethand);
  service.publish(shuffledeck);
  service.publish(addcard);
  service.publish(removecard);
  service.publish(hasset);
  service.publish(getscore);
  service.publish(addscore);
  service.publish(hascard);
  cout << "Starting server..." << endl;
  service.start(settings);
}
