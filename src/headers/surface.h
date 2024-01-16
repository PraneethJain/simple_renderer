#pragma once

#include "common.h"
#include "texture.h"

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
};

struct Triangle
{
    std::array<Vector3f, 3> vertices;
    std::array<Vector3f, 3> normals;
    std::array<Vector2f, 3> uvs;
    Vector3f centroid;
    AABB aabb;
};

struct Surface
{
    std::vector<Triangle> triangles;
    AABB aabb;

    bool isLight;
    uint32_t shapeIdx;

    Vector3f diffuse;
    float alpha;

    Texture diffuseTexture, alphaTexture;

    Interaction rayPlaneIntersect(const Ray &ray, Vector3f p, Vector3f n);
    Interaction rayTriangleIntersect(const Ray &ray, Triangle t);
    Interaction rayIntersect(const Ray &ray);

    Vector3f center()
    {
        return (this->aabb.start + this->aabb.end) / 2;
    }

  private:
    bool hasDiffuseTexture();
    bool hasAlphaTexture();
};

std::vector<Surface> createSurfaces(std::string pathToObj, bool isLight, uint32_t shapeIdx);