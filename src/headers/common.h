#pragma once

#include <array>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "vec.h"

#include "json/include/nlohmann/json.hpp"

struct Ray
{
    Vector3f o, d;
    float t = 1e30f;
    float tmax = 1e30f;

    Ray(Vector3f origin, Vector3f direction, float t = 1e30f, float tmax = 1e30f)
        : o(origin), d(direction), t(t), tmax(tmax){};
};

struct Interaction
{
    Vector3f p, n;
    float t = 1e30f;
    bool didIntersect = false;
};