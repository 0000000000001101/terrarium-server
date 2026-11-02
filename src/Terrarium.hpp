#pragma once

#include "Server.hpp"

class Terrarium
{
public:
    Terrarium() = default;
    virtual ~Terrarium() = default;

    void Run();

private:
    Server NetworkServer;
};
