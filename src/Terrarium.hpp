#pragma once

#include "ThreadSafeQueue.hpp"
#include "NetworkingServer.hpp"

class Terrarium
{
public:
    Terrarium();
    virtual ~Terrarium() = default;

    void Run(const char* ipAddress, const std::uint16_t& port);

private:
    ThreadSafeQueue<IncomingNetworkPacket> IncomingQueue;
    ThreadSafeQueue<OutgoingNetworkPacket> OutgoingQueue;
    NetworkingServer NetworkServer;
};
