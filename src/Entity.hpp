#pragma once

#include <string>

struct Vec2
{
    float x;
    float y;
};

struct Entity
{
    std::string UUID;
    std::string name;
    std::string type;
    std::string image_path;
    std::string description;
    Vec2 position;
    Vec2 size;
    int layer;
    float rotation;
    Vec2 pivot_point;
};
