#ifndef SPACE_HANDLERS_SUBSCRIPTION_HANDLER_H
#define SPACE_HANDLERS_SUBSCRIPTION_HANDLER_H

#include "network.h"
#include "postgres_connector.h"
#include <iostream>

template<uint32_t ROWS, uint32_t COLS>
class SubscriptionHandle {
public:
    using game_ptr = std::shared_ptr<GameManager<ROWS, COLS>>;
    using psql_ref = PostgresConnector&;

private:
    game_ptr game;
    psql_ref postgres;

public:
    SubscriptionHandle(game_ptr game, psql_ref postgres) : game(game), postgres(postgres) {}

    void operator()(Server& server) {
        server.add_http_endpoint("/subscribe", [&](Server::http_request req) -> std::pair<http::status, std::string> {
            return handle_subsciption(req);
        });
    }

private:
    std::pair<http::status, std::string> handle_subsciption(Server::http_request req) {
        std::string body = req.body();
        std::string name = retrieve_field(body, "name");
        std::string color = retrieve_field(body, "color");

        std::string message = "name=" + name + " color=" + color;

        return std::make_pair(http::status::not_found, message);
    }
};

#endif //SPACE_HANDLERS_SUBSCRIPTION_HANDLER_H