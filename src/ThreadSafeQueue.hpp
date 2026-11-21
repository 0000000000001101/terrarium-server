#pragma once

#include <websocketpp/server.hpp>
#include <mutex>
#include <queue>
#include <optional>

struct IncomingNetworkPacket
{
    websocketpp::connection_hdl Connection;
    std::string Payload;
};

struct OutgoingNetworkPacket
{
    std::optional<websocketpp::connection_hdl> Connection = std::nullopt;
    std::string Payload;
    bool IsBroadcast = false;
};

template <typename T>
class ThreadSafeQueue
{
public:
    void Push(T object)
    {
        std::lock_guard<std::mutex> lock(QueueMutex);
        InternalQueue.push(std::move(object));
    }

    std::optional<T> Pop()
    {
        std::lock_guard<std::mutex> lock(QueueMutex);

        if (InternalQueue.empty())
            return std::nullopt;

        T object = std::move(InternalQueue.front());
        InternalQueue.pop();
        return object;
    }

private:
    std::mutex QueueMutex;
    std::queue<T> InternalQueue;
};
