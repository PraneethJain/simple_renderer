#pragma once

#include "common.h"
#include "texture.h"

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