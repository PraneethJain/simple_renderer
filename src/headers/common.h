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

struct AABB
{
    Vector3f start;
    Vector3f end;

    AABB operator|(const AABB &aabb) const
    {
        AABB res{start, end};
        for (int i = 0; i < 3; ++i)
        {
            res.start[i] = std::min(res.start[i], aabb.start[i]);
            res.end[i] = std::max(res.end[i], aabb.end[i]);
        }
        return res;
    }

    AABB &operator|=(const AABB &aabb)
    {
        for (int i = 0; i < 3; ++i)
        {
            this->start[i] = std::min(this->start[i], aabb.start[i]);
            this->end[i] = std::max(this->end[i], aabb.end[i]);
        }

        return *this;
    }

    AABB() : start{Vector3f{INFINITY, INFINITY, INFINITY}}, end{Vector3f{-INFINITY, -INFINITY, -INFINITY}}
    {
    }
    AABB(Vector3f start, Vector3f end) : start{start}, end{end}
    {
    }

    bool rayIntersect(const Ray &ray)
    {
        const Vector3f bmin{start};
        const Vector3f bmax{end};
        float tx1 = (bmin.x - ray.o.x) / ray.d.x, tx2 = (bmax.x - ray.o.x) / ray.d.x;
        float tmin = std::min(tx1, tx2), tmax = std::max(tx1, tx2);
        float ty1 = (bmin.y - ray.o.y) / ray.d.y, ty2 = (bmax.y - ray.o.y) / ray.d.y;
        tmin = std::max(tmin, std::min(ty1, ty2)), tmax = std::min(tmax, std::max(ty1, ty2));
        float tz1 = (bmin.z - ray.o.z) / ray.d.z, tz2 = (bmax.z - ray.o.z) / ray.d.z;
        tmin = std::max(tmin, std::min(tz1, tz2)), tmax = std::min(tmax, std::max(tz1, tz2));
        return tmax >= tmin && tmin < ray.t && tmax > 0;
    }
};

template <class T> struct BVH
{
    std::vector<T *> surfaces;
    AABB aabb{};
    BVH<T> *left{nullptr};
    BVH<T> *right{nullptr};

    BVH<T>()
    {
    }

    BVH<T>(std::vector<T *> surfaces) : surfaces{surfaces}
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
        std::vector<T *> left{}, right{};
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

        this->left = new BVH<T>(left);
        this->right = new BVH<T>(right);
    }

    bool is_leaf()
    {
        return left == nullptr && right == nullptr;
    }
};