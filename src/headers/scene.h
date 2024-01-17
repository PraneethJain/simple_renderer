#pragma once

#include "camera.h"
#include "surface.h"

struct BVHNode
{
    std::vector<Surface *> surfaces;
    AABB aabb{};
    BVHNode *left = nullptr;
    BVHNode *right = nullptr;

    BVHNode()
    {
    }

    BVHNode(std::vector<Surface *> surfaces) : surfaces{surfaces}
    {
        if (surfaces.size() <= 1)
            return;

        for (auto surface : surfaces)
        {
            aabb |= surface->aabb;
        }
        int longest_index{0};
        for (int i{1}; i <= 2; ++i)
        {
            if (aabb.end[i] - aabb.start[i] > aabb.end[longest_index] - aabb.start[longest_index])
            {
                longest_index = i;
            }
        }

        float mid{(aabb.end[longest_index] + aabb.start[longest_index]) / 2};
        std::vector<Surface *> left{}, right{};
        for (auto surf : surfaces)
        {
            if (surf->center()[longest_index] < mid)
            {
                left.push_back(surf);
            }
            else
            {
                right.push_back(surf);
            }
        }

        if (left.empty() ^ right.empty()) // if any one is empty, then terminate
            return;

        this->left = new BVHNode(left);
        this->right = new BVHNode(right);
    }

    bool is_leaf()
    {
        return left == nullptr && right == nullptr;
    }
};

struct Scene
{
    std::vector<Surface> surfaces;
    Camera camera;
    Vector2i imageResolution;

    BVHNode bvh;

    Scene(){};
    Scene(std::string sceneDirectory, std::string sceneJson);
    Scene(std::string pathToJson);

    void parse(std::string sceneDirectory, nlohmann::json sceneConfig);

    Interaction rayIntersect(Ray &ray);
    Interaction bvhIntersect(Ray &ray);
    Interaction bvhTriangleIntersect(Ray &ray);
};