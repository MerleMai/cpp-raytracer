#pragma once

#include <optional>
#include <vector>

#include <Eigen/Core>

#include "geometry/hit.hpp"
#include "geometry/ray.hpp"
#include "scene/scene.hpp"

/**
 * @brief Executes the raytracing algorithm to render a complete 3D scene into a pixel grid.
 * 
 * The Renderer evaluates screen pixels by casting camera rays, detecting surface intersections,
 * calculating direct illumination (ambient, diffuse, specular), handling shadows, and recursively
 * computing secondary reflection and refraction rays.
 */
class Renderer {
private:
    /// @brief Constant reference to the scene data model to be rendered.
    const Scene& scene_;

public:
    /**
     * @brief Constructs a Renderer instance bound to a specific scene.
     * 
     * @param scene Reference to the validated scene containing screen, lights, and object tree.
     */
    explicit Renderer(const Scene& scene);

    /**
     * @brief Renders the entire scene and returns a vector of pixel color values.
     * 
     * Iterates over all pixels of the screen using 2x2 sub-pixel supersampling (anti-aliasing).
     * Traces rays for each sample and averages the resulting colors for each output pixel.
     * The image is split into row ranges that are rendered concurrently across the available
     * hardware threads; each pixel only reads from the (const) scene, so this does not change
     * the result compared to single-threaded rendering.
     * 
     * @return Vector of 3D RGB color vectors in row-major order (width x height).
     */
    std::vector<Eigen::Vector3d> render() const;

private:
    /**
     * @brief Finds the closest intersection point between a ray and any object in the scene.
     * 
     * @param ray The ray to test against all scene objects.
     * 
     * @return std::optional<Hit> Containing the hit information if an intersection occurs, or std::nullopt.
     */
    std::optional<Hit> closestHit(const Ray& ray) const;

    /**
     * @brief Recursively traces a ray through the scene to determine its accumulated color.
     * 
     * Computes direct local shading at the hit point and recursively blends contributions
     * from reflected and refracted secondary rays up to the specified recursion depth.
     * 
     * @param ray The primary or secondary ray being traced.
     * @param depth Current remaining recursion depth for secondary rays.
     * 
     * @return Accumulated 3D RGB color vector along the ray path.
     */
    Eigen::Vector3d trace(const Ray& ray, int depth) const;

    /**
     * @brief Computes local Phong shading (ambient, diffuse, and specular) at an intersection point.
     * 
     * @param hit The surface intersection data including point, normal, and primitive material.
     * @param viewDirection Normalized unit vector pointing from the hit point towards the observer.
     * 
     * @return Shaded 3D RGB color vector resulting from direct illumination and ambient light.
     */
    Eigen::Vector3d shade(const Hit& hit, const Eigen::Vector4d& viewDirection) const;

    /**
     * @brief Determines whether an intersection point is shadowed relative to a specific light source.
     * 
     * Casts a shadow ray towards the light source and checks if any scene object obstructs
     * the light path before reaching the source distance.
     * 
     * @param hit The surface intersection point being evaluated for direct lighting.
     * @param light The point light source being checked.
     * 
     * @return True if the path to the light source is occluded by an object, false otherwise.
     */
    bool inShadow(const Hit& hit, const LightSource& light) const;
};
