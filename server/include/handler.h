#ifndef SPACE_HANDLER_H
#define SPACE_HANDLER_H

#include "network.h"
#include "player.h"
#include "game_manager.h"

template<uint32_t ROWS, uint32_t COLS>
class GameHandler : public WebsocketHandler {
public:

private:
    std::shared_ptr<Player<ROWS, COLS>> player;
    std::shared_ptr<GameManager<ROWS, COLS>> game_manager;

public:
    GameHandler(std::shared_ptr<GameManager<ROWS, COLS>> game_manager) 
        : game_manager{ game_manager } { }

    void on_open(Server::ws_stream_pointer ws, Server::http_request req) override {
        std::cout << "New connection" << std::endl;
        std::cout << "Token: " << req[http::field::authorization] << std::endl;

        std::string token = req[http::field::authorization].to_string();
        player = std::make_shared<Player<ROWS, COLS>>(token);

        game_manager->register_player(player);
    }

    void on_message(const std::string& message) override {
        typename Player<ROWS, COLS>::direction dir = Player<ROWS, COLS>::parse_action(message);
        std::cout << "Direction : " << dir << std::endl;
        // do action !
    }
};

#endif //SPACE_HANDLER_H