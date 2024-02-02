#pragma once

#include "common.h"

enum LightType
{
    POINT_LIGHT = 0,
    DIRECTIONAL_LIGHT = 1,
    NUM_LIGHT_TYPES
};

struct Light
{
    LightType light_type;
    Vector3f radiance;
    Vector3f v;
    Vector3f shade(const Interaction &si, Vector3f c);
};

std::vector<Light> loadLights(const nlohmann::json &sceneConfig);