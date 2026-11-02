#include "Terrarium.hpp"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

void Terrarium::Run(const char* ipAddress, const std::uint16_t& port)
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
        // TODO: Loop over incoming queue.
        //while (/* messages in queue */)
        {
            // Depending on the type of message, there are a few ways the server may respond:
            // - message_type is invalid:
            //   - Discard message, log it. Don't do anything else.
            // - message_type is valid:
            //   - Is it something that affects state?
            //     - Server saves it to scene state.
            //   - Is it something everyone should know?
            //     - Server sends it to everyone.
            //   - Is it a request for the current scene?
            //     - Server sends the full scene to a single client.
            //
            // Those messages the server wants to send, will be added to an outgoing queue.
        }

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
