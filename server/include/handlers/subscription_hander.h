#ifndef SPACE_HANDLERS_SUBSCRIPTION_HANDLER_H
#define SPACE_HANDLERS_SUBSCRIPTION_HANDLER_H

#include "network.h"
#include "postgres_connector.h"
#include <iostream>
#include <string>
#include <sstream>

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

        // validate name and color
        if (name.empty() || color.empty())
            return std::make_pair(http::status::bad_request, "name or color is empty");

        if (name.length() > 15)
            return std::make_pair(http::status::bad_request, "name is too long");

        auto hsl = parse_color(color);
        if (!hsl)
            return std::make_pair(http::status::bad_request, "invalid color");

        return std::make_pair(http::status::created, "");
    }
};

#endif //SPACE_HANDLERS_SUBSCRIPTION_HANDLER_H