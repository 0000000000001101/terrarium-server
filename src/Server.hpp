#pragma once

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <nlohmann/json.hpp>

class Server
{
public:
    Server();
    virtual ~Server();

    void Run(const char* ipAddress, std::uint16_t port);

    void OnOpen(websocketpp::connection_hdl connection);
    void OnFail(websocketpp::connection_hdl connection);
    void OnClose(websocketpp::connection_hdl connection);

    void OnMessage(websocketpp::connection_hdl connection, websocketpp::server<websocketpp::config::asio>::message_ptr message_ptr);

private:
    websocketpp::server<websocketpp::config::asio> Endpoint;
};
