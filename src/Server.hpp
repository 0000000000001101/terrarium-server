#pragma once

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <nlohmann/json.hpp>

class Server
{
    // using ServerEndpoint = websocketpp::server<websocketpp::config::asio>;
    // using websocketpp::connection_hdl;
    // using websocketpp::lib::bind;
    // using websocketpp::lib::placeholders::_1;
    // using websocketpp::lib::placeholders::_2;

public:
    Server();
    virtual ~Server();

    void Run(const char* ipAddress, std::uint16_t port);

private:
    websocketpp::server<websocketpp::config::asio> Endpoint;
};
