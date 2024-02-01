#include "light.h"

std::vector<Light> loadLights(const nlohmann::json &sceneConfig)
{
    std::vector<Light> lights{};
    auto point_lights = sceneConfig["pointLights"];
    for (auto point_light : point_lights)
    {
        auto location = point_light["location"];
        auto radiance = point_light["radiance"];
        lights.push_back({.light_type = POINT_LIGHT,
                          .radiance = Vector3f(radiance[0], radiance[1], radiance[2]),
                          .v = Vector3f(location[0], location[1], location[2])});
    }
    auto directional_lights = sceneConfig["directionalLights"];
    for (auto directional_light : directional_lights)
    {
        auto direction = directional_light["direction"];
        auto radiance = directional_light["radiance"];
        lights.push_back({.light_type = DIRECTIONAL_LIGHT,
                          .radiance = Vector3f(radiance[0], radiance[1], radiance[2]),
                          .v = Vector3f(direction[0], direction[1], direction[2])});
    }
    return lights;
}