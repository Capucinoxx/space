#ifndef SPACE_HANDLERS_SUBSCRIPTION_HANDLER_H
#define SPACE_HANDLERS_SUBSCRIPTION_HANDLER_H

#include "network.h"
#include "postgres_connector.h"
#include "utils.h"
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

        if (ncount_unicode_chars(name) > 15)
            return std::make_pair(http::status::bad_request, "name is too long");

        auto hsl = parse_color(color);
        if (!hsl)
            return std::make_pair(http::status::bad_request, "invalid color");

        std::string query = "INSERT INTO player (name, secret, h, s, l) VALUES ($1, $2, $3, $4, $5)";

        std::string secret = game->generate_secret();

        auto result = postgres.execute(query, 
            name, secret, std::get<0>(*hsl), std::get<1>(*hsl), std::get<2>(*hsl));
        
        return result.affected_rows() == 1 ? 
            std::make_pair(http::status::created, secret) :
            std::make_pair(http::status::bad_request, "failed to insert player");
    }
};

#endif //SPACE_HANDLERS_SUBSCRIPTION_HANDLER_H