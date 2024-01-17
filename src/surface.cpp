#include "surface.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

std::vector<Surface> createSurfaces(std::string pathToObj, bool isLight, uint32_t shapeIdx)
{
    std::string objDirectory;
    const size_t last_slash_idx = pathToObj.rfind('/');
    if (std::string::npos != last_slash_idx)
    {
        objDirectory = pathToObj.substr(0, last_slash_idx);
    }

    std::vector<Surface> surfaces;

    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig reader_config;
    if (!reader.ParseFromFile(pathToObj, reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();
    auto &materials = reader.GetMaterials();

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++)
    {
        Surface surf;
        surf.isLight = isLight;
        surf.shapeIdx = shapeIdx;
        std::set<int> materialIds;

        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            if (fv > 3)
            {
                std::cerr << "Not a triangle mesh" << std::endl;
                exit(1);
            }

            // Loop over vertices in the face. Assume 3 vertices per-face
            std::array<Vector3f, 3> vertices{}, normals{};
            std::array<Vector2f, 3> uvs{};
            for (size_t v = 0; v < fv; v++)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0)
                {
                    tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

                    normals[v] = Vector3f(nx, ny, nz);
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0)
                {
                    tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

                    uvs[v] = Vector2f(tx, ty);
                }

                vertices[v] = Vector3f(vx, vy, vz);
            }

            const auto centroid = std::accumulate(vertices.begin(), vertices.end(), Vector3f{}) / 3;

            surf.triangles.push_back({
                vertices,
                normals,
                uvs,
            });

            // per-face material
            materialIds.insert(shapes[s].mesh.material_ids[f]);

            index_offset += fv;
        }

        if (materialIds.size() > 1)
        {
            std::cerr << "One of the meshes has more than one material. This is not allowed." << std::endl;
            exit(1);
        }

        if (materialIds.size() == 0)
        {
            std::cerr << "One of the meshes has no material definition, may cause unexpected behaviour." << std::endl;
        }
        else
        {
            // Load textures from Materials
            auto matId = *materialIds.begin();
            if (matId != -1)
            {
                auto mat = materials[matId];

                surf.diffuse = Vector3f(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
                if (mat.diffuse_texname != "")
                    surf.diffuseTexture = Texture(objDirectory + "/" + mat.diffuse_texname);

                surf.alpha = mat.specular[0];
                if (mat.alpha_texname != "")
                    surf.alphaTexture = Texture(objDirectory + "/" + mat.alpha_texname);
            }
        }

        surfaces.push_back(surf);
        shapeIdx++;
    }

    return surfaces;
}

bool Surface::hasDiffuseTexture()
{
    return this->diffuseTexture.data != 0;
}

bool Surface::hasAlphaTexture()
{
    return this->alphaTexture.data != 0;
}

Interaction Surface::rayPlaneIntersect(const Ray &ray, Vector3f p, Vector3f n)
{
    Interaction si;

    float dDotN = Dot(ray.d, n);
    if (dDotN != 0.f)
    {
        float t = -Dot((ray.o - p), n) / dDotN;

        if (t >= 0.f)
        {
            si.didIntersect = true;
            si.t = t;
            si.n = n;
            si.p = ray.o + ray.d * si.t;
        }
    }

    return si;
}

// Interaction Surface::rayTriangleIntersect(Ray ray, Vector3f v1, Vector3f v2, Vector3f v3, Vector3f n)
Interaction Surface::rayTriangleIntersect(const Ray &ray, Triangle t)
{
    auto v1 = t.vertices[0];
    auto v2 = t.vertices[1];
    auto v3 = t.vertices[2];
    auto n = Normalize(std::accumulate(t.normals.begin(), t.normals.end(), Vector3f{}));
    Interaction si = this->rayPlaneIntersect(ray, v1, n);

    if (si.didIntersect)
    {
        bool edge1 = false, edge2 = false, edge3 = false;

        // Check edge 1
        {
            Vector3f nIp = Cross((si.p - v1), (v3 - v1));
            Vector3f nTri = Cross((v2 - v1), (v3 - v1));
            edge1 = Dot(nIp, nTri) > 0;
        }

        // Check edge 2
        {
            Vector3f nIp = Cross((si.p - v1), (v2 - v1));
            Vector3f nTri = Cross((v3 - v1), (v2 - v1));
            edge2 = Dot(nIp, nTri) > 0;
        }

        // Check edge 3
        {
            Vector3f nIp = Cross((si.p - v2), (v3 - v2));
            Vector3f nTri = Cross((v1 - v2), (v3 - v2));
            edge3 = Dot(nIp, nTri) > 0;
        }

        if (edge1 && edge2 && edge3)
        {
            // Intersected triangle!
            si.didIntersect = true;
        }
        else
        {
            si.didIntersect = false;
        }
    }

    return si;
}

Interaction Surface::rayIntersect(const Ray &ray)
{
    Interaction siFinal;
    float tmin = ray.t;

    if (this->aabb.rayIntersect(ray))
    {
        for (auto triangle : this->triangles)
        {
            if (triangle.aabb.rayIntersect(ray))
            {
                Interaction si = this->rayTriangleIntersect(ray, triangle);
                if (si.t <= tmin && si.didIntersect)
                {
                    siFinal = si;
                    tmin = si.t;
                }
            }
        }
    }

    return siFinal;
}

Interaction Surface::bvhIntersect(const Ray &ray)
{
    Interaction siFinal;
    float tmin = ray.t;
    std::vector<BVHTriangleNode *> stack{&this->bvh};
    while (!stack.empty())
    {
        BVHTriangleNode *cur{stack.back()};
        stack.pop_back();
        if (cur->aabb.rayIntersect(ray))
        {
            if (cur->is_leaf())
            {
                for (auto triangle : cur->triangles)
                {
                    Interaction si = this->rayTriangleIntersect(ray, *triangle);
                    if (si.t <= tmin && si.didIntersect)
                    {
                        siFinal = si;
                        tmin = si.t;
                    }
                }
            }
            else
            {
                if (cur->left != nullptr)
                {
                    stack.push_back(cur->left);
                }
                if (cur->right != nullptr)
                {
                    stack.push_back(cur->right);
                }
            }
        }
    }

    return siFinal;
}