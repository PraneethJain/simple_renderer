#pragma once

#include "scene.h"

struct Integrator
{
    Integrator(Scene &scene, int spp);

    long long render();
    int spp;
    Scene scene;
    Texture outputImage;
};