#include "Server.hpp"
#include <spdlog/spdlog.h>

Server::Server()
{
    // TODO: Allow certain logging.
    Endpoint.clear_access_channels(websocketpp::log::alevel::all);
    Endpoint.clear_error_channels(websocketpp::log::elevel::all);

    // Initializes the networking backend.
    Endpoint.init_asio();

    Endpoint.set_open_handler(websocketpp::lib::bind(&Server::OnOpen, this, websocketpp::lib::placeholders::_1));
    Endpoint.set_fail_handler(websocketpp::lib::bind(&Server::OnFail, this, websocketpp::lib::placeholders::_1));
    Endpoint.set_close_handler(websocketpp::lib::bind(&Server::OnClose, this, websocketpp::lib::placeholders::_1));
    Endpoint.set_message_handler(websocketpp::lib::bind(&Server::OnMessage, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
    // TODO: More callbacks. https://docs.websocketpp.org/reference_8handlers.html
    // - Ping Handler (to trace log it)
    // - Pong Handler (to trace log it)
    // - Pong Timeout Handler (to trace log it)
    // - HTTP Handler (to 301 or 308 it to the Terrarium website)
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

    // Queue a command to start accepting connections.
    Endpoint.start_accept();

    // Runs the ASIO event loop, processes network stuff and dispatches things to callbacks. This is a blocking call.
    Endpoint.run();

    // If we get here we either exited cleanly, or encountered some problem.
}

void Server::OnOpen(websocketpp::connection_hdl connection)
{
    websocketpp::lib::error_code error;
    const auto connectionPtr = Endpoint.get_con_from_hdl(connection, error);
    if (error)
    {
        spdlog::warn("Server::OnOpen(): unable to access connection: {}", error.message());
        return;
    }

    const auto& request = connectionPtr->get_request();
    spdlog::trace(
        "Server::OnOpen(): remoteEndpoint={} resource='{}' method='{}' uri='{}' version='{}'",
        connectionPtr->get_remote_endpoint(), connectionPtr->get_resource(), request.get_method(), request.get_uri(), request.get_version()
    );

    for (const auto& header : request.get_headers())
    {
        spdlog::trace("Server::OnOpen(): header '{}': '{}'", header.first, header.second);
    }



    std::string userAgent = "NULL";
    try
    {
        userAgent = request.get_headers().at("User-Agent");
    }
    catch (const std::out_of_range& e)
    {
        spdlog::debug("No User-Agent in connection! ({})", e.what());
    }

    spdlog::info("Client connected.\n\tIP: `{}`\n\tUser-Agent: `{}`\n\tResource: `{}`\n", connectionPtr->get_remote_endpoint(), userAgent, connectionPtr->get_resource());
}

void Server::OnFail(websocketpp::connection_hdl connection)
{
    websocketpp::lib::error_code error;
    const auto connectionPtr = Endpoint.get_con_from_hdl(connection, error);
    if (error)
    {
        spdlog::warn("Server::OnFail(): unable to access connection: {}", error.message());
        return;
    }

    const auto& request = connectionPtr->get_request();
    spdlog::trace(
        "Server::OnFail(): remoteEndpoint={} resource='{}' method='{}' uri='{}' version='{}'",
        connectionPtr->get_remote_endpoint(), connectionPtr->get_resource(), request.get_method(), request.get_uri(), request.get_version()
    );

    for (const auto& header : request.get_headers())
    {
        spdlog::trace("Server::OnFail(): header '{}': '{}'", header.first, header.second);
    }



    std::string userAgent = "NULL";
    try
    {
        userAgent = request.get_headers().at("User-Agent");
    }
    catch (const std::out_of_range& e)
    {
        spdlog::debug("No User-Agent in connection! ({})", e.what());
    }

    spdlog::info("Client failed to connect.\n\tIP: `{}`\n\tUser-Agent: `{}`\n\tResource: `{}`\n", connectionPtr->get_remote_endpoint(), userAgent, connectionPtr->get_resource());
}

void Server::OnClose(websocketpp::connection_hdl connection)
{
    websocketpp::lib::error_code error;
    const auto connectionPtr = Endpoint.get_con_from_hdl(connection, error);
    if (error)
    {
        spdlog::warn("Server::OnClose(): unable to access connection: {}", error.message());
        return;
    }

    const auto& request = connectionPtr->get_request();
    spdlog::trace(
        "Server::OnClose(): remoteEndpoint={} resource='{}' method='{}' uri='{}' version='{}'",
        connectionPtr->get_remote_endpoint(), connectionPtr->get_resource(), request.get_method(), request.get_uri(), request.get_version()
    );

    for (const auto& header : request.get_headers())
    {
        spdlog::trace("Server::OnClose(): header '{}': '{}'", header.first, header.second);
    }



    std::string userAgent = "NULL";
    try
    {
        userAgent = request.get_headers().at("User-Agent");
    }
    catch (const std::out_of_range& e)
    {
        spdlog::debug("No User-Agent in connection! ({})", e.what());
    }

    spdlog::info("Connection closed.\n\tIP: `{}`\n\tUser-Agent: `{}`\n\tResource: `{}`\n", connectionPtr->get_remote_endpoint(), userAgent, connectionPtr->get_resource());
}

void Server::OnMessage(websocketpp::connection_hdl connection, websocketpp::server<websocketpp::config::asio>::message_ptr message_ptr)
{
    websocketpp::lib::error_code error;
    const auto connectionPtr = Endpoint.get_con_from_hdl(connection, error);
    if (error)
    {
        spdlog::warn("Server::OnMessage(): unable to access connection: {}", error.message());
        return;
    }

    const std::string& remoteEndpoint = connectionPtr->get_remote_endpoint();

    const auto& request = connectionPtr->get_request();
    spdlog::trace(
        "Server::OnMessage(): remoteEndpoint={} resource='{}' method='{}' uri='{}' version='{}'",
        remoteEndpoint, connectionPtr->get_resource(), request.get_method(), request.get_uri(), request.get_version()
    );

    for (const auto& header : request.get_headers())
    {
        spdlog::trace("Server::OnMessage(): header '{}': '{}'", header.first, header.second);
    }



    std::string userAgent = "NULL";
    try
    {
        userAgent = request.get_headers().at("User-Agent");
    }
    catch (const std::out_of_range& e)
    {
        spdlog::debug("No User-Agent in connection! ({})", e.what());
    }

    spdlog::info("Message recieved!\n\tIP: `{}`\n\tUser-Agent: `{}`\n\tResource: {}\n\tMessage:\n{}\n", connectionPtr->get_remote_endpoint(), userAgent, connectionPtr->get_resource(), message_ptr->get_payload());
}
