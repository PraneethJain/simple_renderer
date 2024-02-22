#pragma once

#include "scene.h"

struct Integrator
{
    Integrator(Scene &scene, int spp, int sampling_strategy);

    long long render();
    int spp;
    int sampling_strategy;
    Scene scene;
    Texture outputImage;
};