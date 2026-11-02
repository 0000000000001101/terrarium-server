#pragma once

#include "Server.hpp"

class Terrarium
{
public:
    Terrarium() = default;
    virtual ~Terrarium() = default;

    void Run(const char* ipAddress, const std::uint16_t& port);

private:
    Server NetworkServer;
};
