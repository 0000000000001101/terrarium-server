#include "NetworkingServer.hpp"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

NetworkingServer::NetworkingServer(ThreadSafeQueue<IncomingNetworkPacket>& IncomingQueue, ThreadSafeQueue<OutgoingNetworkPacket>& OutgoingQueue)
    : IncomingQueue(IncomingQueue), OutgoingQueue(OutgoingQueue)
{
    // TODO: Allow certain logging.
    Endpoint.clear_access_channels(websocketpp::log::alevel::all);
    Endpoint.clear_error_channels(websocketpp::log::elevel::all);

    // Initializes the networking backend.
    Endpoint.init_asio();

    Endpoint.set_open_handler(websocketpp::lib::bind(&NetworkingServer::OnOpen, this, websocketpp::lib::placeholders::_1));
    Endpoint.set_fail_handler(websocketpp::lib::bind(&NetworkingServer::OnFail, this, websocketpp::lib::placeholders::_1));
    Endpoint.set_close_handler(websocketpp::lib::bind(&NetworkingServer::OnClose, this, websocketpp::lib::placeholders::_1));
    Endpoint.set_message_handler(websocketpp::lib::bind(&NetworkingServer::OnMessage, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
    // TODO: More callbacks. https://docs.websocketpp.org/reference_8handlers.html
    // - Ping Handler (to trace log it)
    // - Pong Handler (to trace log it)
    // - Pong Timeout Handler (to trace log it)
    // - HTTP Handler (to 301 or 308 it to the Terrarium website)
}

void NetworkingServer::Run(const char* ipAddress, const std::uint16_t& port)
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

    // Runs the ASIO event loop, processes network stuff and dispatches things to callbacks.
    // This is a blocking call.
    Endpoint.run();
}

void NetworkingServer::Stop()
{
    Endpoint.stop_listening();

    // TODO: Close all connections? stop_listening does not close them.
    // Check WebSocket++ docs.
    // ...

    Endpoint.stop();
}

void NetworkingServer::OnOpen(websocketpp::connection_hdl connection)
{
    websocketpp::lib::error_code error;
    const auto connectionPtr = Endpoint.get_con_from_hdl(connection, error);
    if (error)
    {
        spdlog::warn("NetworkingServer::OnOpen(): unable to access connection: {}", error.message());
        return;
    }

    const auto& request = connectionPtr->get_request();
    spdlog::trace(
        "NetworkingServer::OnOpen(): remoteEndpoint={} resource='{}' method='{}' uri='{}' version='{}'",
        connectionPtr->get_remote_endpoint(), connectionPtr->get_resource(), request.get_method(), request.get_uri(), request.get_version()
    );

    for (const auto& header : request.get_headers())
    {
        spdlog::trace("NetworkingServer::OnOpen(): header '{}': '{}'", header.first, header.second);
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

    ActiveConnections.insert(connection);
    spdlog::info("Client connected.\n\tIP: `{}`\n\tUser-Agent: `{}`\n\tResource: `{}`\n", connectionPtr->get_remote_endpoint(), userAgent, connectionPtr->get_resource());
}

void NetworkingServer::OnFail(websocketpp::connection_hdl connection)
{
    websocketpp::lib::error_code error;
    const auto connectionPtr = Endpoint.get_con_from_hdl(connection, error);
    if (error)
    {
        spdlog::warn("NetworkingServer::OnFail(): unable to access connection: {}", error.message());
        return;
    }

    const auto& request = connectionPtr->get_request();
    spdlog::trace(
        "NetworkingServer::OnFail(): remoteEndpoint={} resource='{}' method='{}' uri='{}' version='{}'",
        connectionPtr->get_remote_endpoint(), connectionPtr->get_resource(), request.get_method(), request.get_uri(), request.get_version()
    );

    for (const auto& header : request.get_headers())
    {
        spdlog::trace("NetworkingServer::OnFail(): header '{}': '{}'", header.first, header.second);
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

void NetworkingServer::OnClose(websocketpp::connection_hdl connection)
{
    websocketpp::lib::error_code error;
    const auto connectionPtr = Endpoint.get_con_from_hdl(connection, error);
    if (error)
    {
        spdlog::warn("NetworkingServer::OnClose(): unable to access connection: {}", error.message());
        return;
    }

    const auto& request = connectionPtr->get_request();
    spdlog::trace(
        "NetworkingServer::OnClose(): remoteEndpoint={} resource='{}' method='{}' uri='{}' version='{}'",
        connectionPtr->get_remote_endpoint(), connectionPtr->get_resource(), request.get_method(), request.get_uri(), request.get_version()
    );

    for (const auto& header : request.get_headers())
    {
        spdlog::trace("NetworkingServer::OnClose(): header '{}': '{}'", header.first, header.second);
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

    ActiveConnections.erase(connection);
    spdlog::info("Connection closed.\n\tIP: `{}`\n\tUser-Agent: `{}`\n\tResource: `{}`\n", connectionPtr->get_remote_endpoint(), userAgent, connectionPtr->get_resource());
}

void NetworkingServer::OnMessage(websocketpp::connection_hdl connection, websocketpp::server<websocketpp::config::asio>::message_ptr message_ptr)
{
    websocketpp::lib::error_code error;
    const auto connectionPtr = Endpoint.get_con_from_hdl(connection, error);
    if (error)
    {
        spdlog::warn("NetworkingServer::OnMessage(): unable to access connection: {}", error.message());
        return;
    }

    const std::string& remoteEndpoint = connectionPtr->get_remote_endpoint();

    const auto& request = connectionPtr->get_request();
    spdlog::trace(
        "NetworkingServer::OnMessage(): remoteEndpoint={} resource='{}' method='{}' uri='{}' version='{}'",
        remoteEndpoint, connectionPtr->get_resource(), request.get_method(), request.get_uri(), request.get_version()
    );

    for (const auto& header : request.get_headers())
    {
        spdlog::trace("NetworkingServer::OnMessage(): header '{}': '{}'", header.first, header.second);
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

    // Package the message and push it to a thread-safe queue for another thread.
    IncomingNetworkPacket packet;
    packet.Connection = connection;
    packet.Payload = message_ptr->get_payload();
    IncomingQueue.Push(packet);
}

void NetworkingServer::SendMessage(websocketpp::connection_hdl connection, const std::string &message)
{
    websocketpp::lib::error_code error;
    const auto connectionPtr = Endpoint.get_con_from_hdl(connection, error);
    if (error)
    {
        spdlog::warn("NetworkingServer::SendMessage(): Connection is no longer valid!! It was not cleaned up? ({})", error.message());
        return;
    }

    spdlog::trace("Sending message to {} payload: {}", connectionPtr->get_remote_endpoint(), message);
    try
    {
        Endpoint.send(connection, message, websocketpp::frame::opcode::text);
    }
    catch(const websocketpp::exception& e)
    {
        spdlog::warn("Failed to send message!: {}", e.what());
    }
    catch(const std::exception& e)
    {
        spdlog::warn(e.what());
    }
}

void NetworkingServer::BroadcastMessage(const std::string &message, std::optional<websocketpp::connection_hdl> excludeConnection)
{
    if (excludeConnection.has_value())
    {
        websocketpp::lib::error_code error;
        const auto connectionPtr = Endpoint.get_con_from_hdl(excludeConnection.value(), error);
        if (error)
        {
            spdlog::warn("NetworkingServer::SendMessage(): Connection is no longer valid!! It was not cleaned up? ({})", error.message());
            return;
        }
        spdlog::trace("Broadcasting to all, except for `{}`.", connectionPtr->get_remote_endpoint());
    }
    else
        spdlog::trace("Broadcasting to all.");

    for (const websocketpp::connection_hdl& connection : ActiveConnections)
    {
        // If there is a connection to exclude, compare both weak_ptr's "control block" to see if they have the same management metadata or smt smt. If so, skip/exclude.
        if (excludeConnection.has_value() && !excludeConnection.value().owner_before(connection) && !connection.owner_before(excludeConnection.value()))
            continue;

        SendMessage(connection, message);
    }
}

void NetworkingServer::RequestQueueFlush()
{
    spdlog::trace("Requesting ASIO to flush outgoing queue.");
    Endpoint.get_io_service().post([this]() { ProcessOutgoingQueue(); });
}

void NetworkingServer::ProcessOutgoingQueue()
{
    spdlog::trace("Flushing outgoing message queue.");
    while (auto packet = OutgoingQueue.Pop())
    {
        if (packet->IsBroadcast)
            BroadcastMessage(packet->Payload, packet->Connection);
        else
            SendMessage(packet->Connection.value(), packet->Payload);
    }
}
