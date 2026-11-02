#pragma once

#include "NetworkingServer.hpp"
#include "ThreadSafeQueue.hpp"

class Terrarium
{
public:
    Terrarium() = default;
    virtual ~Terrarium() = default;

    void Run(const char* ipAddress, const std::uint16_t& port);

private:
    NetworkingServer NetworkServer;
    ThreadSafeQueue<IncomingNetworkPacket> IncomingQueue;
    ThreadSafeQueue<OutgoingNetworkPacket> OutgoingQueue;
};
