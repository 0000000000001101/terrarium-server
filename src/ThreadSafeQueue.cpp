#include "ThreadSafeQueue.hpp"

template <typename T>
void ThreadSafeQueue<T>::Push(T object)
{
    std::lock_guard<std::mutex> lock(QueueMutex);
    InternalQueue.push(std::move(object));
}

template <typename T>
std::optional<T> ThreadSafeQueue<T>::Pop()
{
    std::lock_guard<std::mutex> lock(QueueMutex);

    if (InternalQueue.empty())
        return std::nullopt;

    T object = std::move(InternalQueue.front());
    InternalQueue.pop();
    return object;
}
