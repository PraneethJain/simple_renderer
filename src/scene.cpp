#include "scene.h"

Scene::Scene(std::string sceneDirectory, std::string sceneJson)
{
    nlohmann::json sceneConfig;
    try
    {
        sceneConfig = nlohmann::json::parse(sceneJson);
    }
    catch (std::runtime_error e)
    {
        std::cerr << "Could not parse json." << std::endl;
        exit(1);
    }

    this->parse(sceneDirectory, sceneConfig);
}

Scene::Scene(std::string pathToJson)
{
    std::string sceneDirectory;

#ifdef _WIN32
    const size_t last_slash_idx = pathToJson.rfind('\\');
#else
    const size_t last_slash_idx = pathToJson.rfind('/');
#endif

    if (std::string::npos != last_slash_idx)
    {
        sceneDirectory = pathToJson.substr(0, last_slash_idx);
    }

    nlohmann::json sceneConfig;
    try
    {
        std::ifstream sceneStream(pathToJson.c_str());
        sceneStream >> sceneConfig;
    }
    catch (std::runtime_error e)
    {
        std::cerr << "Could not load scene .json file." << std::endl;
        exit(1);
    }

    this->parse(sceneDirectory, sceneConfig);
}

void Scene::parse(std::string sceneDirectory, nlohmann::json sceneConfig)
{
    // Output
    try
    {
        auto res = sceneConfig["output"]["resolution"];
        this->imageResolution = Vector2i(res[0], res[1]);
    }
    catch (nlohmann::json::exception e)
    {
        std::cerr << "\"output\" field with resolution, filename & spp should be defined in the scene file."
                  << std::endl;
        exit(1);
    }

    // Cameras

    Vector3f offset{};
    try
    {
        auto cam = sceneConfig["camera"];
        auto from = Vector3f(cam["from"][0], cam["from"][1], cam["from"][2]);
        auto to = Vector3f(cam["to"][0], cam["to"][1], cam["to"][2]);
        auto up = Vector3f(cam["up"][0], cam["up"][1], cam["up"][2]);

        offset = from;

        this->camera = Camera(from - offset, to - offset, up, float(cam["fieldOfView"]), this->imageResolution);
    }
    catch (nlohmann::json::exception e)
    {
        std::cerr << "No camera(s) defined. Atleast one camera should be defined." << std::endl;
        exit(1);
    }

    // Surface
    try
    {
        auto surfacePaths = sceneConfig["surface"];

        uint32_t surfaceIdx = 0;
        for (std::string surfacePath : surfacePaths)
        {
            surfacePath = sceneDirectory + "/" + surfacePath;
            auto surf = createSurfaces(surfacePath, /*isLight=*/false, /*idx=*/surfaceIdx);
            for (auto &s : surf)
            {
                for (auto &v : s.triangles)
                {
                    for (int i = 0; i < 3; ++i)
                    {
                        v.vertices[i] -= offset;
                        for (int j = 0; j < 3; ++j)
                        {
                            v.aabb.start[j] = std::min(v.aabb.start[j], v.vertices[i][j]);
                            v.aabb.end[j] = std::max(v.aabb.end[j], v.vertices[i][j]);
                        }
                    }
                    s.aabb |= v.aabb;
                }
            }
            this->surfaces.insert(this->surfaces.end(), surf.begin(), surf.end());

            surfaceIdx = surfaceIdx + surf.size();
        }
    }
    catch (nlohmann::json::exception e)
    {
        std::cout << "No surfaces defined." << std::endl;
    }

    // Construct BVH from the scene
    std::vector<Surface *> surf_pointers{};
    for (auto &surface : this->surfaces)
    {
        std::vector<Triangle *> tri_pointers{};
        for (auto &triangle : surface.triangles)
        {
            tri_pointers.push_back(&triangle);
        }
        surface.bvh = BVH<Triangle>(tri_pointers);
        surf_pointers.push_back(&surface);
    }
    this->bvh = BVH<Surface>(surf_pointers);
}

Interaction Scene::bvhIntersect(Ray &ray)
{
    Interaction siFinal;
    std::vector<BVH<Surface> *> stack{&this->bvh};
    while (!stack.empty())
    {
        auto *cur{stack.back()};
        stack.pop_back();
        if (cur->aabb.rayIntersect(ray))
        {
            if (cur->is_leaf())
            {
                for (auto &surface : cur->surfaces)
                {
                    Interaction si = surface->bvhIntersect(ray);
                    if (si.t <= ray.t)
                    {
                        siFinal = si;
                        ray.t = si.t;
                    }
                }
            }
            else
            {
                bool REMOVE_THIS{false};
                if (cur->left != nullptr)
                {
                    REMOVE_THIS = true;
                    stack.push_back(cur->left);
                }
                if (cur->right != nullptr)
                {
                    REMOVE_THIS = true;
                    stack.push_back(cur->right);
                }
                assert(REMOVE_THIS);
            }
        }
    }

    return siFinal;
}

Interaction Scene::rayIntersect(Ray &ray)
{
    Interaction siFinal;

    for (auto &surface : this->surfaces)
    {
        Interaction si = surface.rayIntersect(ray);
        if (si.t <= ray.t)
        {
            siFinal = si;
            ray.t = si.t;
        }
    }

    return siFinal;
}