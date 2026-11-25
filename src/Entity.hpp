#pragma once

#include <yaml-cpp/yaml.h>
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

namespace YAML
{
    template<>
    struct convert<Vec2>
    {
        static Node encode(const Vec2& vec2)
        {
            Node node;
            node["x"] = vec2.x;
            node["y"] = vec2.y;

            return node;
        };

        static bool decode(const Node& node, Vec2& vec2)
        {
            if (!node.IsMap() || !node["x"] || !node["y"])
            {
                return false;
            }
            vec2.x = node["x"].as<float>();
            vec2.y = node["y"].as<float>();

            return true;
        };
    };

    template<>
    struct convert<Entity>
    {
        static Node encode(const Entity& entity)
        {
            Node node;
            node["UUID"] = entity.UUID;
            node["name"] = entity.name;
            node["type"] = entity.type;
            node["image_path"] = entity.image_path;
            node["description"] = entity.description;
            node["position"] = entity.position;
            node["size"] = entity.size;
            node["layer"] = entity.layer;
            node["rotation"] = entity.rotation;
            node["pivot_point"] = entity.pivot_point;

            return node;
        };

        static bool decode(const Node& node, Entity& entity)
        {
            if (!node.IsMap() || !node["UUID"] || !node["name"] || !node["type"] || !node["image_path"] || !node["description"] || !node["position"] || !node["size"] || !node["layer"] || !node["rotation"] || !node["pivot_point"])
            {
                return false;
            }
            entity.UUID = node["UUID"].as<std::string>();
            entity.name = node["name"].as<std::string>();
            entity.type = node["type"].as<std::string>();
            entity.image_path = node["image_path"].as<std::string>();
            entity.description = node["description"].as<std::string>();
            entity.position = node["position"].as<Vec2>();
            entity.size = node["size"].as<Vec2>();
            entity.layer = node["layer"].as<int>();
            entity.rotation = node["rotation"].as<float>();
            entity.pivot_point = node["pivot_point"].as<Vec2>();

            return true;
        };
    };
}