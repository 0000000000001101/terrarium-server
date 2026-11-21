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
                    // TODO: Send real state.

                    // Test state.
                    if (true)
                    {
                        {
                            OutgoingNetworkPacket outgoingPacket;
                            outgoingPacket.Connection = incomingPacket->Connection;
                            outgoingPacket.Payload = "{\"description\":\"null\",\"image_path\":\"res://assets/animals/angry hedgehog.png\",\"layer\":0,\"message_type\":\"asset_created\",\"pivot_point\":{\"x\":469.229614257813,\"y\":141.178894042969},\"position\":{\"x\":396.729125976563,\"y\":68.6788330078125},\"rotation\":0,\"size\":{\"x\":145.0009765625,\"y\":145.000122070313},\"type\":\"2f6075b6-3303-6755-68e1-e89f6b0113f6\",\"uuid\":\"2f6075b6-3303-6755-68e1-e89f6b0113f6\"}";
                            OutgoingQueue.Push(outgoingPacket);
                        }
                        {
                            OutgoingNetworkPacket outgoingPacket;
                            outgoingPacket.Connection = incomingPacket->Connection;
                            outgoingPacket.Payload = "{\"description\":\"null\",\"image_path\":\"res://assets/animals/alabama beach mouse.jpg\",\"layer\":0,\"message_type\":\"asset_created\",\"pivot_point\":{\"x\":536.471618652344,\"y\":285.077941894531},\"position\":{\"x\":488.386627197266,\"y\":212.577835083008},\"rotation\":0,\"size\":{\"x\":96.1700134277344,\"y\":145.000228881836},\"type\":\"f2cb4aaf-0572-db1f-fb31-b69820c9d3ad\",\"uuid\":\"f2cb4aaf-0572-db1f-fb31-b69820c9d3ad\"}";
                            OutgoingQueue.Push(outgoingPacket);
                        }
                        {
                            OutgoingNetworkPacket outgoingPacket;
                            outgoingPacket.Connection = incomingPacket->Connection;
                            outgoingPacket.Payload = "{\"description\":\"null\",\"image_path\":\"res://assets/animals/raccoon.jpg\",\"layer\":0,\"message_type\":\"asset_created\",\"pivot_point\":{\"x\":877.354370117188,\"y\":97.6569137573242},\"position\":{\"x\":768.603759765625,\"y\":25.156867980957},\"rotation\":0,\"size\":{\"x\":217.501159667969,\"y\":145.000091552734},\"type\":\"1e011ba5-2a19-44f5-76c5-d59336201bf2\",\"uuid\":\"1e011ba5-2a19-44f5-76c5-d59336201bf2\"}";
                            OutgoingQueue.Push(outgoingPacket);
                        }
                        {
                            OutgoingNetworkPacket outgoingPacket;
                            outgoingPacket.Connection = incomingPacket->Connection;
                            outgoingPacket.Payload = "{\"description\":\"null\",\"image_path\":\"res://assets/animals/short eared elephant shrew.jpg\",\"layer\":0,\"message_type\":\"asset_created\",\"pivot_point\":{\"x\":691.317138671875,\"y\":516.823852539063},\"position\":{\"x\":571.180236816406,\"y\":444.323791503906},\"rotation\":0,\"size\":{\"x\":240.273864746094,\"y\":145.000122070313},\"type\":\"395255a9-4c09-a02d-e128-256711d2936b\",\"uuid\":\"395255a9-4c09-a02d-e128-256711d2936b\"}";
                            OutgoingQueue.Push(outgoingPacket);
                        }
                        {
                            OutgoingNetworkPacket outgoingPacket;
                            outgoingPacket.Connection = incomingPacket->Connection;
                            outgoingPacket.Payload = "{\"description\":\"null\",\"image_path\":\"res://assets/animals/storm the bat.jpg\",\"layer\":0,\"message_type\":\"asset_created\",\"pivot_point\":{\"x\":869.351135253906,\"y\":348.039672851563},\"position\":{\"x\":796.851135253906,\"y\":275.539642333984},\"rotation\":0,\"size\":{\"x\":145.0,\"y\":145.000091552734},\"type\":\"7978b7f2-e787-a6f2-287f-4f30dab2e05f\",\"uuid\":\"7978b7f2-e787-a6f2-287f-4f30dab2e05f\"}";
                            OutgoingQueue.Push(outgoingPacket);
                        }
                        {
                            OutgoingNetworkPacket outgoingPacket;
                            outgoingPacket.Connection = incomingPacket->Connection;
                            outgoingPacket.Payload = "{\"description\":\"null\",\"image_path\":\"res://assets/animals/angry hedgehog.png\",\"layer\":0,\"message_type\":\"asset_created\",\"pivot_point\":{\"x\":453.175628662109,\"y\":408.982574462891},\"position\":{\"x\":380.675415039063,\"y\":336.482513427734},\"rotation\":0,\"size\":{\"x\":145.000427246094,\"y\":145.000122070313},\"type\":\"3bb32320-d416-ab2c-032e-cb183c8d1fd1\",\"uuid\":\"3bb32320-d416-ab2c-032e-cb183c8d1fd1\"}";
                            OutgoingQueue.Push(outgoingPacket);
                        }
                        {
                            OutgoingNetworkPacket outgoingPacket;
                            outgoingPacket.Connection = incomingPacket->Connection;
                            outgoingPacket.Payload = "{\"description\":\"null\",\"image_path\":\"res://assets/animals/angry hedgehog.png\",\"layer\":0,\"message_type\":\"asset_created\",\"pivot_point\":{\"x\":764.13330078125,\"y\":274.171936035156},\"position\":{\"x\":691.633239746094,\"y\":201.671844482422},\"rotation\":0,\"size\":{\"x\":145.000061035156,\"y\":145.000152587891},\"type\":\"33df5312-81f1-d0b9-4b4f-d59cdd14737d\",\"uuid\":\"33df5312-81f1-d0b9-4b4f-d59cdd14737d\"}";
                            OutgoingQueue.Push(outgoingPacket);
                        }
                    }
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
