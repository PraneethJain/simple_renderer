#pragma once

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
    uint32_t surfIdx = UINT32_MAX;
    uint32_t triIdx = UINT32_MAX;
    Vector2f uv;
};

struct AABB
{
    Vector3f min = Vector3f(1e30f, 1e30f, 1e30f);
    Vector3f max = Vector3f(-1e30f, -1e30f, -1e30f);
    Vector3f centroid = Vector3f(0.f, 0.f, 0.f);

    bool intersects(Ray ray)
    {
        float tx1 = (min.x - ray.o.x) / ray.d.x, tx2 = (max.x - ray.o.x) / ray.d.x;
        float tmin = std::min(tx1, tx2), tmax = std::max(tx1, tx2);
        float ty1 = (min.y - ray.o.y) / ray.d.y, ty2 = (max.y - ray.o.y) / ray.d.y;
        tmin = std::max(tmin, std::min(ty1, ty2)), tmax = std::min(tmax, std::max(ty1, ty2));
        float tz1 = (min.z - ray.o.z) / ray.d.z, tz2 = (max.z - ray.o.z) / ray.d.z;
        tmin = std::max(tmin, std::min(tz1, tz2)), tmax = std::min(tmax, std::max(tz1, tz2));
        return tmax >= tmin && tmin < ray.t && tmax > 0;
    }
};

struct Tri
{
    Vector3f v1, v2, v3;
    Vector2f uv1, uv2, uv3;
    Vector3f normal;
    Vector3f centroid;

    AABB bbox;

    Vector2f getUV(Vector3f p)
    {
        const float fullArea{Tri::area(v1, v2, v3)};
        const float gamma{Tri::area(v1, v2, p) / fullArea};
        const float alpha{Tri::area(v2, v3, p) / fullArea};
        const float beta{Tri::area(v3, v1, p) / fullArea};
        return alpha * uv1 + beta * uv2 + gamma * uv3;
    }

    static float area(Vector3f v1, Vector3f v2, Vector3f v3)
    {
        return Cross(v1 - v2, v1 - v3).Length() / 2.f;
    }
};

struct BVHNode
{
    AABB bbox;
    uint32_t left = 0, right = 0;
    uint32_t firstPrim = 0, primCount = 0;
};