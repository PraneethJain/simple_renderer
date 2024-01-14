#pragma once

#include "common.h"
#include "texture.h"

struct Triangle
{
    std::array<Vector3f, 3> vertices;
    std::array<Vector3f, 3> normals;
    std::array<Vector2f, 3> uvs;
    Vector3f centroid;
};

struct Surface
{
    std::vector<Triangle> triangles;

    bool isLight;
    uint32_t shapeIdx;

    Vector3f diffuse;
    float alpha;

    Texture diffuseTexture, alphaTexture;

    Interaction rayPlaneIntersect(Ray ray, Vector3f p, Vector3f n);
    Interaction rayTriangleIntersect(Ray ray, Vector3f v1, Vector3f v2, Vector3f v3, Vector3f n);
    Interaction rayIntersect(Ray ray);

private:
    bool hasDiffuseTexture();
    bool hasAlphaTexture();
};

std::vector<Surface> createSurfaces(std::string pathToObj, bool isLight, uint32_t shapeIdx);