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
    // Returns false if queue is closed.
    bool Push(T object)
    {
        std::lock_guard<std::mutex> lock(QueueMutex);

        if (Closed)
            return false; // Can't push data if the queue is closed!

        InternalQueue.push(std::move(object));

        // There is data available! Notify anyone waiting for data.
        DataAvailable.notify_one();

        return true;
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

    // Waits until the queue has data or, when the queue is closed.
    // Returns true if there is data, false only when closed AND there is no more data.
    bool WaitForData()
    {
        std::unique_lock<std::mutex> lock(QueueMutex);

        // The condition variable releases QueueMutex while the thread sleeps.
        // When awakened, it reacquires the mutex and checks this.
        DataAvailable.wait(lock, [this]
        {
            return Closed || !InternalQueue.empty();
        });

        // If closed but items remain, return true so they can be processed.
        return !InternalQueue.empty();
    }

    // Stops accepting items into the queue, and wakes anyone waiting for data.
    void Close()
    {
        {
            std::unique_lock<std::mutex> lock(QueueMutex);
            Closed = true;
        }

        DataAvailable.notify_all();
    }

private:
    std::mutex QueueMutex;
    std::condition_variable DataAvailable;
    std::queue<T> InternalQueue;
    bool Closed = false;
};
