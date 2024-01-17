#pragma once

#include "common.h"
#include "texture.h"

struct Triangle
{
    std::array<Vector3f, 3> vertices;
    std::array<Vector3f, 3> normals;
    std::array<Vector2f, 3> uvs;
    AABB aabb;
    Vector3f center()
    {
        return (aabb.start + aabb.end) / 2;
    }
};

struct BVHTriangleNode
{
    std::vector<Triangle *> triangles;
    AABB aabb{};
    BVHTriangleNode *left = nullptr;
    BVHTriangleNode *right = nullptr;

    BVHTriangleNode()
    {
    }

    BVHTriangleNode(std::vector<Triangle *> triangles) : triangles{triangles}
    {
        if (triangles.size() <= 1)
            return;

        for (auto triangle : triangles)
        {
            aabb |= triangle->aabb;
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
        std::vector<Triangle *> left{}, right{};
        for (auto tri : triangles)
        {
            if (tri->center()[longest_index] < mid)
            {
                left.push_back(tri);
            }
            else
            {
                right.push_back(tri);
            }
        }

        if (left.empty() ^ right.empty()) // if any one is empty, then terminate
            return;

        this->left = new BVHTriangleNode(left);
        this->right = new BVHTriangleNode(right);
    }

    bool is_leaf()
    {
        return left == nullptr && right == nullptr;
    }
};

struct Surface
{
    std::vector<Triangle> triangles;
    AABB aabb;
    BVHTriangleNode bvh;

    bool isLight;
    uint32_t shapeIdx;

    Vector3f diffuse;
    float alpha;

    Texture diffuseTexture, alphaTexture;

    Interaction rayPlaneIntersect(const Ray &ray, Vector3f p, Vector3f n);
    Interaction rayTriangleIntersect(const Ray &ray, Triangle t);
    Interaction rayIntersect(const Ray &ray);
    Interaction bvhIntersect(const Ray &ray);

    Vector3f center()
    {
        return (this->aabb.start + this->aabb.end) / 2;
    }

  private:
    bool hasDiffuseTexture();
    bool hasAlphaTexture();
};

std::vector<Surface> createSurfaces(std::string pathToObj, bool isLight, uint32_t shapeIdx);