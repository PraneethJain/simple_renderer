#include "render.h"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    const double ERROR = 5e-6;

    for (int x{0}; x < this->scene.imageResolution.x; ++x)
    {
        for (int y{0}; y < this->scene.imageResolution.y; ++y)
        {
            Ray cameraRay{this->scene.camera.generateRay(x, y)};
            Interaction si{this->scene.rayIntersect(cameraRay)};
            Vector3f color{};
            for (auto light : this->scene.lights)
            {
                if (light.light_type == POINT_LIGHT)
                {
                    auto w{light.v - si.p};
                    auto normalized_w{Normalize(w)};
                    Ray shadowRay{si.p + ERROR * si.n, normalized_w};
                    Interaction shadow{this->scene.rayIntersect(shadowRay)};
                    if ((not shadow.didIntersect) or (shadow.t > (light.v - si.p).Length()))
                    {
                        color += light.radiance * Dot(normalized_w, si.n) / (w.LengthSquared() * M_PI);
                    }
                }
                else if (light.light_type == DIRECTIONAL_LIGHT)
                {
                    Ray shadowRay{si.p + ERROR * si.n, light.v};
                    if (not this->scene.rayIntersect(shadowRay).didIntersect)
                    {
                        color += light.radiance * Dot(si.n, light.v) / M_PI;
                    }
                }
            }
            this->outputImage.writePixelColor(color, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path>";
        return 1;
    }
    Scene scene(argv[1]);

    Integrator rayTracer(scene);
    auto renderTime = rayTracer.render();

    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}
