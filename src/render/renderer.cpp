#include "render/renderer.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <thread>

#include "geometry/constants.hpp"
#include "geometry/primitiveObject.hpp"
#include "render/color.hpp"

Renderer::Renderer(const Scene& scene)
    : scene_(scene) {}

std::optional<Hit> Renderer::closestHit(const Ray& ray) const {
    std::optional<Hit> closest;

    for (const std::unique_ptr<Object>& object : scene_.getObjects()) {
        for (const Hit& hit : object->intersect(ray)) {
            if (!closest || hit.t < closest->t) {
                closest = hit;
            }
        }
    }

    return closest;
}

Eigen::Vector3d Renderer::trace(const Ray& ray, int depth) const {
    const std::optional<Hit> hit = closestHit(ray);
    if (!hit) {
        return Eigen::Vector3d(0.0, 0.0, 0.0);
    }

    Eigen::Vector4d viewDirection = -ray.getDirection();
    viewDirection.head<3>().normalize();

    Eigen::Vector3d color = shade(*hit, viewDirection);

    if (depth <= 0) {
        return color;
    }

    const Material& material = hit->primitive->getMaterial();

    Eigen::Vector4d incoming = ray.getDirection();
    incoming.head<3>().normalize();

    const Eigen::Vector4d normal = hit->entering ? hit->normal : Eigen::Vector4d(-hit->normal);
    const double incomingDotNormal = incoming.head<3>().dot(normal.head<3>());

    Eigen::Vector4d reflected = incoming - 2.0 * incomingDotNormal * normal;
    reflected.head<3>().normalize();
    color = combine(
        color,
        material.getReflected().cwiseProduct(trace(Ray(hit->point, reflected), depth - 1))
    );

    const double firstIndex = hit->entering ? scene_.getMedium().getIndex() : hit->primitive->getIndex();
    const double secondIndex = hit->entering ? hit->primitive->getIndex() : scene_.getMedium().getIndex();
    const double indexRatio = firstIndex / secondIndex;
    const double radicand = 1.0 - indexRatio * indexRatio * (1.0 - incomingDotNormal * incomingDotNormal);

    if (radicand >= 0.0) {
        Eigen::Vector4d refracted = indexRatio * incoming - (indexRatio * incomingDotNormal + std::sqrt(radicand)) * normal;
        refracted.head<3>().normalize();
        color = combine(
            color,
            material.getRefracted().cwiseProduct(trace(Ray(hit->point, refracted), depth - 1))
        );
    }

    return color;
}

Eigen::Vector3d Renderer::shade(const Hit& hit, const Eigen::Vector4d& viewDirection) const {
    const Material& material = hit.primitive->getMaterial();
    const Eigen::Vector4d normal = hit.entering ? hit.normal : Eigen::Vector4d(-hit.normal);

    Eigen::Vector3d color = material.getAmbient().cwiseProduct(scene_.getMedium().getAmbient());

    for (const LightSource& light : scene_.getSources()) {
        if (inShadow(hit, light)) {
            continue;
        }

        Eigen::Vector4d toLight = light.getPosition() - hit.point;
        toLight.w() = 0.0;
        toLight.head<3>().normalize();

        const double lightDotNormal = toLight.head<3>().dot(normal.head<3>());
        if (lightDotNormal <= 0.0) {
            continue;
        }

        const Eigen::Vector3d diffuse = material.getDiffuse().cwiseProduct(light.getIntensity()) * lightDotNormal;
        color = combine(color, diffuse);

        const Eigen::Vector4d reflected = 2.0 * lightDotNormal * normal - toLight;
        const double reflectedDotView = std::max(0.0, reflected.head<3>().dot(viewDirection.head<3>()));
        const Eigen::Vector3d specular = material.getSpecular().cwiseProduct(light.getIntensity()) * std::pow(reflectedDotView, material.getShininess());
        color = combine(color, specular);
    }

    return color;
}

bool Renderer::inShadow(const Hit& hit, const LightSource& light) const {
    Eigen::Vector4d toLight = light.getPosition() - hit.point;
    toLight.w() = 0.0;
    const double distance = toLight.head<3>().norm();
    toLight.head<3>().normalize();

    const std::optional<Hit> blocker = closestHit(Ray(hit.point, toLight));
    return blocker.has_value() && blocker->t < distance - GEOMETRY_EPSILON;
}

std::vector<Eigen::Vector3d> Renderer::render() const {
    const Screen& screen = scene_.getScreen();
    const int width = screen.getWidth();
    const int height = screen.getHeight();

    std::vector<Eigen::Vector3d> pixels(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));

    constexpr int samplesPerAxis = 2;
    const double sampleStep = 1.0 / static_cast<double>(samplesPerAxis);
    const int sampleCount = samplesPerAxis * samplesPerAxis;
    const int recursionDepth = scene_.getMedium().getRecursion();

    // Renders a contiguous range of rows [rowStart, rowEnd). Independent
    // pixels only read from the (const) scene, so multiple rows can safely
    // be rendered concurrently on disjoint slices of `pixels`.
    const auto renderRows = [&](int rowStart, int rowEnd) {
        for (int row = rowStart; row < rowEnd; row++) {
            for (int column = 0; column < width; column++) {
                Eigen::Vector3d colorSum(0.0, 0.0, 0.0);

                for (int sampleRow = 0; sampleRow < samplesPerAxis; sampleRow++) {
                    for (int sampleColumn = 0; sampleColumn < samplesPerAxis; sampleColumn++) {
                        const double offsetX = (sampleColumn + 0.5) * sampleStep;
                        const double offsetY = (sampleRow + 0.5) * sampleStep;
                        const Ray ray = screen.rayFor(column, row, offsetX, offsetY);
                        colorSum += trace(ray, recursionDepth);
                    }
                }

                pixels[static_cast<std::size_t>(row) * static_cast<std::size_t>(width) + static_cast<std::size_t>(column)] =
                    colorSum / sampleCount;
            }
        }
    };

    const unsigned int hardwareThreads = std::thread::hardware_concurrency();
    const unsigned int threadCount = std::max(1u, hardwareThreads == 0 ? 4u : hardwareThreads);

    if (threadCount <= 1 || height <= 1) {
        renderRows(0, height);
        return pixels;
    }

    const int rowsPerThread = (height + static_cast<int>(threadCount) - 1) / static_cast<int>(threadCount);

    std::vector<std::thread> workers;
    workers.reserve(threadCount);

    for (unsigned int threadIndex = 0; threadIndex < threadCount; threadIndex++) {
        const int rowStart = static_cast<int>(threadIndex) * rowsPerThread;
        const int rowEnd = std::min(height, rowStart + rowsPerThread);
        if (rowStart >= rowEnd) {
            break;
        }
        workers.emplace_back(renderRows, rowStart, rowEnd);
    }

    for (std::thread& worker : workers) {
        worker.join();
    }

    return pixels;
}
