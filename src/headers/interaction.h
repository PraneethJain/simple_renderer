#pragma once

#include "vec.h"

// Forward declaration of BSDF class
class BSDF;

struct Interaction
{
    // Position of interaction
    Vector3f p;
    // Normal of the surface at interaction
    Vector3f n;
    // The uv co-ordinates at the intersection point
    Vector2f uv;
    // The viewing direction in local shading frame
    Vector3f wi;
    // Distance of intersection point from origin of the ray
    float t = 1e30f;
    // Used for light intersection, holds the radiance emitted by the emitter.
    Vector3f emissiveColor = Vector3f(0.f, 0.f, 0.f);
    // BSDF at the shading point
    BSDF *bsdf;
    // Vectors defining the orthonormal basis
    Vector3f a, b, c;

    bool didIntersect = false;

    Vector3f toWorld(Vector3f w)
    {
        Vector3f res{};
        std::vector<Vector3f> ONB{a, b, c};
        for (int i{0}; i < 3; ++i)
            for (int j{0}; j < 3; ++j)
                res[i] += ONB[j][i] * w[j];
        return res;
    }

    Vector3f toLocal(Vector3f w)
    {
        return {Dot(a, w), Dot(b, w), Dot(c, w)};
    }
};