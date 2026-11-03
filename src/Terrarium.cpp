#include "Terrarium.hpp"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

Terrarium::Terrarium()
    : NetworkServer(IncomingQueue, OutgoingQueue)
{
}

void Terrarium::Run(const char *ipAddress, const std::uint16_t &port)
{
    // Launch networking worker thread.
    // std::thread networkWorker(&Server::Run, &NetworkServer);
    std::thread networkWorker(
        [this, &ipAddress, &port]()
        {
            NetworkServer.Run(ipAddress, port);
        }
    );

    // Main game loop.
    const int TICK_RATE = 60;
    const auto MS_PER_TICK = std::chrono::milliseconds(1000 / TICK_RATE);

    bool serverRunning = true;
    while (serverRunning)
    {
        auto tickStart = std::chrono::steady_clock::now();
        // spdlog::trace("Tick!");

        // Process all incoming messages.
        while (std::optional<IncomingNetworkPacket> incomingPacket = IncomingQueue.Pop())
        {
            try
            {
                nlohmann::json json = nlohmann::json::parse(incomingPacket->Payload);
                std::string message_type = json.at("message_type").get<std::string>();

                if (message_type == "meow")
                {
                    OutgoingNetworkPacket outgoingPacket;
                    outgoingPacket.Connection = incomingPacket->Connection;
                    outgoingPacket.Payload = "meow :3";
                    OutgoingQueue.Push(outgoingPacket);
                }
                else if (message_type == "request_state")
                {
                    // TODO: Send state.
                }
                // These remaining message types are the ones to broadcast to all but sender.
                else if (
                    message_type == "glass_selected" ||
                    message_type == "asset_created" ||
                    message_type == "asset_deleted" ||
                    message_type == "asset_layer_reordered" ||
                    message_type == "asset_moved" ||
                    message_type == "asset_resized" ||
                    message_type == "asset_rotated"
                )
                {
                    // TODO: Update state of tracked assets/entities.

                    OutgoingNetworkPacket outgoingPacket;
                    outgoingPacket.Connection = incomingPacket->Connection;
                    outgoingPacket.Payload = incomingPacket->Payload;
                    outgoingPacket.IsBroadcast = true;
                    OutgoingQueue.Push(outgoingPacket);
                }
                else
                {
                    spdlog::warn("Invalid message_type recieved! How do I handle \"{}\"?", message_type);
                }
            }
            catch(const nlohmann::json::parse_error& e)
            {
                spdlog::warn("JSON parse error:\n\t\"{}\"", e.what());
                spdlog::warn("Could not parse message as JSON! Discarding message...");
            }
            catch(const std::out_of_range& e)
            {
                spdlog::warn("std::out_of_range:\n\t\"{}\"", e.what());
                spdlog::warn("JSON message did not have a required key! Discarding...");
            }
            catch(const nlohmann::json::type_error& e)
            {
                spdlog::warn("nlohmann::json::type_error:\n\t\"{}\"", e.what());
                spdlog::warn("A key returned an unexpected value type! Discarding...");
            }
            catch(const std::exception& e)
            {
                spdlog::warn("std::exception:\n\t\"{}\"", e.what());
            }
        }
        // After we queue up our messages, we should tell the server to flush it, to send our messages. Messages are batched! :D
        NetworkServer.RequestQueueFlush();

        // Terrarium Server logic.
        // So after dealing with all messages this tick, what do we need to do?
        // Maybe every once in a while, we save the canvas from memory to disk? Offload canvases which aren't active anymore?
        // If this was like other games, things like updating entity positions and physics would be done.

        // Tick rate limiter.
        auto tickEnd = std::chrono::steady_clock::now();
        auto tickDuration = tickEnd - tickStart;
        if (tickDuration < MS_PER_TICK)
            std::this_thread::sleep_for(MS_PER_TICK - tickDuration);
        else
            spdlog::warn("Terrarium Server running {}ms behind! That time will not be made up.", (tickDuration - MS_PER_TICK).count());
    }

    NetworkServer.Stop();

    if (networkWorker.joinable())
    {
        spdlog::info("Waiting for network thread to finish...");
        networkWorker.join();
        spdlog::info("Network thread finished!");
    }
    else
        spdlog::error("Network thread is not joinable? What happened?");
}
