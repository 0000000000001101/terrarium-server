#pragma once

#include "ThreadSafeQueue.hpp"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <nlohmann/json.hpp>

#include <set>

class NetworkingServer
{
public:
    NetworkingServer(ThreadSafeQueue<IncomingNetworkPacket>& IncomingQueue, ThreadSafeQueue<OutgoingNetworkPacket>& OutgoingQueue);
    virtual ~NetworkingServer() = default;

    void Run(const char* ipAddress, const std::uint16_t& port);
    void Stop();

    void OnOpen(websocketpp::connection_hdl connection);
    void OnFail(websocketpp::connection_hdl connection);
    void OnClose(websocketpp::connection_hdl connection);
    void OnMessage(websocketpp::connection_hdl connection, websocketpp::server<websocketpp::config::asio>::message_ptr message_ptr);

    void SendMessage(websocketpp::connection_hdl connection, const std::string& message);
    void BroadcastMessage(const std::string& message, std::optional<websocketpp::connection_hdl> excludeConnection = std::nullopt);

private:
    websocketpp::server<websocketpp::config::asio> Endpoint;
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> ActiveConnections;

    ThreadSafeQueue<IncomingNetworkPacket>& IncomingQueue;
    ThreadSafeQueue<OutgoingNetworkPacket>& OutgoingQueue;
};
