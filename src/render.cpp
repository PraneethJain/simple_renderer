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
            Vector3f result{};
            for (int _ = 0; _ < this->spp; ++_)
            {
                Ray cameraRay = this->scene.camera.generateRandomRay(x, y);
                Interaction si = this->scene.rayIntersect(cameraRay);
                if (si.didIntersect)
                {
                    if (this->sampling_strategy == 0)
                    {
                        Vector3f local_dir{uniform_sample_hemisphere()};
                        Vector3f global_dir{si.toWorld(local_dir)};

                        Ray lightRay{si.p + 1e-3f * si.n, global_dir};
                        Interaction siLight{this->scene.rayEmitterIntersect(lightRay)};
                        Interaction siShadow{this->scene.rayIntersect(lightRay)};

                        if (siLight.didIntersect and siLight.t < siShadow.t)
                        {
                            result += 2 * M_PIf * si.bsdf->eval(&si, local_dir) * siLight.emissiveColor *
                                      std::abs(Dot(si.n, global_dir));
                        }
                    }
                    else if (this->sampling_strategy == 1)
                    {
                        Vector3f local_dir{weighted_sample_cosine()};
                        Vector3f global_dir{si.toWorld(local_dir)};

                        Ray lightRay{si.p + 1e-3f * si.n, global_dir};
                        Interaction siLight{this->scene.rayEmitterIntersect(lightRay)};
                        Interaction siShadow{this->scene.rayIntersect(lightRay)};

                        if (siLight.didIntersect and siLight.t < siShadow.t)
                        {
                            result += M_PIf * si.bsdf->eval(&si, local_dir) * siLight.emissiveColor;
                        }
                    }
                    else if (this->sampling_strategy == 2)
                    {
                        Vector3f radiance;
                        LightSample ls;
                        int n{static_cast<int>(this->scene.lights.size())};
                        Light light{this->scene.lights[static_cast<int>(std::floor(next_float() * n))]};
                        std::tie(radiance, ls) = light.sample(&si);

                        Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);

                        if (!siShadow.didIntersect || siShadow.t > ls.d)
                        {
                            result += n * si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                        }
                    }
                }
            }
            result /= this->spp;
            Ray centerRay = this->scene.camera.generateRay(x, y);
            Interaction siDirectLight = this->scene.rayEmitterIntersect(centerRay);
            Interaction siDirect = this->scene.rayIntersect(centerRay);
            if (siDirectLight.didIntersect and siDirectLight.t < siDirect.t)
            {
                result = siDirectLight.emissiveColor;
            }
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
