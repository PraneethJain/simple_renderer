#pragma once

#include "scene.h"

struct Integrator
{
    Integrator(Scene &scene);

    long long render(int variant);

    Scene scene;
    Texture outputImage;
};