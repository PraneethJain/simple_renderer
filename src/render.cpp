#include "render.h"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int x{0}; x < scene.imageResolution.x; ++x)
    {
        for (int y{0}; y < scene.imageResolution.y; ++y)
        {
            Ray cameraRay{scene.camera.generateRay(x, y)};
            Interaction si{scene.rayIntersect(cameraRay)};
            Vector3f color{};
            for (auto light : scene.lights)
            {
                if (scene.lightIntersect(si, light) && si.surfIdx != UINT32_MAX && si.triIdx != UINT32_MAX)
                {
                    Vector3f textureColor{1, 1, 1};
                    if (scene.surfaces[si.surfIdx].hasDiffuseTexture())
                    {
                        textureColor = scene.surfaces[si.surfIdx].diffuseTexture.fetch(
                            scene.surfaces[si.surfIdx].tris[si.triIdx], si.p, this->scene.interpolation_variant);
                    }

                    color += light.shade(si, textureColor);
                }
            }
            outputImage.writePixelColor(color, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path> <interpolation_variant>";
        return 1;
    }
    Scene scene(argv[1]);
    scene.interpolation_variant = static_cast<int>(strtol(argv[3], NULL, 10));
    Integrator rayTracer(scene);
    auto renderTime = rayTracer.render();

    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}
