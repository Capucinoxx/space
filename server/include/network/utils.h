#ifndef SPACE_NETWORK_UTILS_H
#define SPACE_NETWORK_UTILS_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using error_code = boost::system::error_code;

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = boost::beast::websocket;

#endif //SPACE_NETWORK_UTILS_H