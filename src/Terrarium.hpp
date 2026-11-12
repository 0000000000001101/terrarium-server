#pragma once

#include "ThreadSafeQueue.hpp"
#include "NetworkingServer.hpp"
#include "Entity.hpp"
#include <unordered_map>

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

    // TODO: Decide if the Terrarium scene/state should be its own class or not.
    std::unordered_map<std::string, Entity> SceneEntities;
};
