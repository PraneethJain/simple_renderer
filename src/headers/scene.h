#pragma once

#include "camera.h"
#include "surface.h"

struct Scene
{
    std::vector<Surface> surfaces;
    Camera camera;
    Vector2i imageResolution;

    BVH<Surface> bvh;

    Scene(){};
    Scene(std::string sceneDirectory, std::string sceneJson);
    Scene(std::string pathToJson);

    void parse(std::string sceneDirectory, nlohmann::json sceneConfig);

    Interaction rayIntersect(Ray &ray, int variant);
    Interaction rayBVHIntersect(Ray &ray, int variant);
};