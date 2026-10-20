#include "Server.hpp"
#include <spdlog/spdlog.h>

Server::Server()
{
    // Initializes the networking backend.
    Endpoint.init_asio();

    // TODO: Setup callbacks.
}

Server::~Server()
{
}

void Server::Run(const char* ipAddress, std::uint16_t port)
{
    websocketpp::lib::asio::error_code addressError;
    const auto address = websocketpp::lib::asio::ip::make_address(ipAddress, addressError);

    if (addressError)
    {
        spdlog::error("Invalid bind IP '{}': {}", ipAddress, addressError.message());
        throw std::runtime_error("Invalid bind IP: '" + std::string(ipAddress) + "': " + addressError.message());
    }

    websocketpp::lib::error_code listenError;
    // Creates the listening socket, and binds to an address and port.
    Endpoint.listen(websocketpp::lib::asio::ip::tcp::endpoint(address, port), listenError);
    if (listenError)
    {
        spdlog::error("Failed to bind {}:{}: {}", address.to_string(), port, listenError.message());
        throw std::runtime_error("Failed to bind: '" + std::string(ipAddress) + ":"  + std::to_string(port) + "': " + listenError.message());
    }
    spdlog::info("Server listening on: `ws://{}:{}`", address.to_string(), port);

    // Server accept loop. Asynchronously accepts new connections.
    // They only get processed after the Endpoint.run();
    Endpoint.start_accept();

    // Runs the ASIO event loop, processes network stuff and dispatches things to callbacks. Blocking call.
    Endpoint.run();

    // If we get here, we either exited cleanly, or encountered some problem.
}
