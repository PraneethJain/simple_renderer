#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>

// Utility Functions
inline float next_float()
{
    // Returns a random real in [0,1).
    return (float)(rand() / (RAND_MAX + 1.0));
}

inline Vector3f uniform_sample_hemisphere()
{
    float theta{acosf(next_float())};
    float phi{2 * M_PIf * next_float()};

    return {sinf(theta) * cosf(phi), sinf(theta) * sinf(phi), cosf(theta)};
}

inline Vector3f weighted_sample_cosine()
{
    float theta{acosf(sqrtf(1.f - next_float()))};
    float phi{2 * M_PIf * next_float()};

    return {sinf(theta) * cosf(phi), sinf(theta) * sinf(phi), cosf(theta)};
}