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
                nlohmann::json jsonMessage = nlohmann::json::parse(incomingPacket->Payload);
                std::string messageType = jsonMessage.at("message_type").get<std::string>();

                if (messageType == "meow")
                {
                    OutgoingNetworkPacket outgoingPacket;
                    outgoingPacket.Connection = incomingPacket->Connection;
                    outgoingPacket.Payload = "meow :3";
                    OutgoingQueue.Push(outgoingPacket);
                }
                else if (messageType == "request_state")
                {
                    for (const auto& entity : SceneEntities)
                    {
                        OutgoingNetworkPacket outgoingPacket;
                        outgoingPacket.Connection = incomingPacket->Connection;
                        nlohmann::json payload = {
                            {"message_type", "asset_created"},
                            {"uuid", entity.second.UUID},
                            {"name", entity.second.name},
                            {"type", entity.second.type},
                            {"image_path", entity.second.image_path},
                            {"description", entity.second.description},
                            {"position", {{"x", entity.second.position.x}, {"y", entity.second.position.y}}},
                            {"size", {{"x", entity.second.size.x}, {"y", entity.second.size.y}}},
                            {"layer", entity.second.layer},
                            {"rotation", entity.second.rotation},
                            {"pivot_point", {{"x", entity.second.pivot_point.x}, {"y", entity.second.pivot_point.y}}}
                        };
                        spdlog::debug("Sending: {}", payload.dump());
                        outgoingPacket.Payload = payload.dump();
                        OutgoingQueue.Push(outgoingPacket);
                    }
                }
                // These remaining message types are the ones to broadcast to all but sender.
                else if (
                    messageType == "glass_selected" ||
                    messageType == "asset_created" ||
                    messageType == "asset_deleted" ||
                    messageType == "asset_layer_reordered" ||
                    messageType == "asset_moved" ||
                    messageType == "asset_resized" ||
                    messageType == "asset_rotated"
                )
                {
                    std::string targetUUID = jsonMessage.at("uuid").get<std::string>();
                    SceneEntities[targetUUID].UUID = targetUUID;
                    for (auto& [key, value] : jsonMessage.items())
                    {
                        spdlog::debug("Key: `{}` Value `{}`", key, value.dump());

                        if (key == "name")
                        {
                            SceneEntities[targetUUID].name = value.get<std::string>();
                        }
                        else if (key == "type")
                        {
                            SceneEntities[targetUUID].type = value.get<std::string>();
                        }
                        else if (key == "image_path")
                        {
                            SceneEntities[targetUUID].image_path = value.get<std::string>();
                        }
                        else if (key == "description")
                        {
                            SceneEntities[targetUUID].description = value.get<std::string>();
                        }
                        else if (key == "position")
                        {
                            SceneEntities[targetUUID].position.x = value["x"].get<float>();
                            SceneEntities[targetUUID].position.y = value["y"].get<float>();
                        }
                        else if (key == "size")
                        {
                            SceneEntities[targetUUID].size.x = value["x"].get<float>();
                            SceneEntities[targetUUID].size.y = value["y"].get<float>();
                        }
                        else if (key == "layer")
                        {
                            SceneEntities[targetUUID].layer = value.get<int>();
                        }
                        else if (key == "rotation")
                        {
                            SceneEntities[targetUUID].rotation = value.get<float>();
                        }
                        else if (key == "pivot_point")
                        {
                            SceneEntities[targetUUID].pivot_point.x = value["x"].get<float>();
                            SceneEntities[targetUUID].pivot_point.y = value["y"].get<float>();
                        }
                        else if (key == "message_type" || key == "uuid")
                        {
                            // Ignore these keys.
                            // This `else if` prevents us from logging "unexpected" keys in the `else` block below.
                        }
                        else
                        {
                            spdlog::warn("Unexpected key found in JSON message! Key: `{}` Value: `{}`", key, value.dump());
                        }
                    }

                    // Send the received packet to others. Thus, only sending the changes.
                    OutgoingNetworkPacket outgoingPacket;
                    outgoingPacket.Connection = incomingPacket->Connection;
                    outgoingPacket.Payload = incomingPacket->Payload;
                    outgoingPacket.IsBroadcast = true;
                    OutgoingQueue.Push(outgoingPacket);
                }
                else
                {
                    spdlog::warn("Invalid message_type recieved! How do I handle \"{}\"?", messageType);
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
