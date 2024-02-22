#include "render.h"

Integrator::Integrator(Scene &scene, int spp, int sampling_strategy)
{
    this->scene = scene;
    this->spp = spp;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
    this->sampling_strategy = sampling_strategy;
}

long long Integrator::render()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f result(0, 0, 0);
            for (int _ = 0; _ < this->spp; ++_)
            {
                Ray cameraRay = this->scene.camera.generateRandomRay(x, y);
                Interaction si = this->scene.rayIntersect(cameraRay);
                result += this->scene.rayEmitterIntersect(cameraRay).emissiveColor;

                if (si.didIntersect)
                {
                    Vector3f local_dir{uniform_sample_hemisphere()};
                    Vector3f global_dir{si.toWorld(local_dir)};

                    Ray lightRay{si.p + 1e-3f * si.n, global_dir};
                    Interaction siLight{this->scene.rayEmitterIntersect(lightRay)};
                    Interaction siShadow{this->scene.rayIntersect(lightRay)};

                    if (siLight.didIntersect and siLight.t < siShadow.t)
                    {
                        result +=
                            si.bsdf->eval(&si, local_dir) * siLight.emissiveColor * std::abs(Dot(si.n, global_dir));
                    }
                }
            }
            result *= 2 * M_PIf / this->spp;
            this->outputImage.writePixelColor(result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path> <num_samples> <sampling_strategy>";
        return 1;
    }
    Scene scene(argv[1]);

    int spp = atoi(argv[3]);
    int sampling_strategy = atoi(argv[4]);
    Integrator rayTracer(scene, spp, sampling_strategy);
    auto renderTime = rayTracer.render();

    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}
