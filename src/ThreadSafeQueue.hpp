#pragma once

#include <websocketpp/server.hpp>
#include <mutex>
#include <queue>
#include <optional>
#include <condition_variable>

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

enum class QueueWaitResult
{
    Data, // Returned from QueueWait, because there is data to process.
    Timeout, // Timeout has been hit. Wake up and do some periodic stuff.
    Closed // Queue closed! Networking thread may be finished!
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

    // Waits until there is data, or until a timeout is hit.
    template <typename Clock, typename Duration>
    QueueWaitResult WaitUntil(std::chrono::time_point<Clock, Duration> deadline)
    {
        std::unique_lock<std::mutex> lock(QueueMutex);

        const bool awakened = DataAvailable.wait_until(
            lock,
            deadline,
            [this]
            {
                return Closed || !InternalQueue.empty();
            });

        if (!awakened)
            return QueueWaitResult::Timeout;

        if (!InternalQueue.empty())
            return QueueWaitResult::Data;

        return QueueWaitResult::Closed;
    };

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
