#pragma once

#include <Eigen/Core>

class PrimitiveObject;

/**
 * @brief Holds detailed ray-primitive intersection data.
 * 
 * Stores the distance parameter $t$ along the ray, the spatial intersection point,
 * the surface normal vector, a pointer to the struck primitive object, and whether
 * the ray is entering or exiting the material volume.
 */
struct Hit {
    /// @brief Distance parameter along the ray direction vector.
    double t = 0.0;

    /// @brief World-space intersection point in homogeneous coordinates
    Eigen::Vector4d point = Eigen::Vector4d::Zero();

    /// @brief Surface normal vector at the point of intersection in homogeneous coordinates.
    Eigen::Vector4d normal = Eigen::Vector4d::Zero();

    /// @brief Pointer to the primitive geometric object that was struck by the ray.
    const PrimitiveObject* primitive = nullptr;

    /// @brief Indicates whether the ray is entering (true) or exiting (false) the primitive's volume
    bool entering = true;
};
