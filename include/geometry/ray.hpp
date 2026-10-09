#pragma once

#include <Eigen/Core>

/**
 * @brief Represents a semi-infinite 3D ray defined by an origin point and a direction vector.
 * 
 * Used throughout the rendering pipeline for primary camera rays, shadow testing,
 * secondary reflections, and refractions.
 */
class Ray {
private:
    /// @brief Starting point of the ray in homogeneous coordinates.
    Eigen::Vector4d origin_;

    /// @brief Direction vector of the ray in homogeneous coordinates.
    Eigen::Vector4d direction_;

public:
    /**
     * @brief Constructs a Ray from its geometric parameters.
     * 
     * @param origin Starting point of the ray.
     * @param direction Direction vector of the ray.
     */
    Ray(
        const Eigen::Vector4d& origin,
        const Eigen::Vector4d& direction
    );

   /**
     * @brief Provides access to the origin point of the ray.
     * 
     * @return Constant reference to the origin vector in homogeneous coordinates.
     */
    const Eigen::Vector4d& getOrigin() const;

    /**
     * @brief Provides access to the direction vector of the ray.
     * 
     * @return Constant reference to the direction vector in homogeneous coordinates.
     */
    const Eigen::Vector4d& getDirection() const;

    /**
     * @brief Evaluates the position along the ray at a given distance parameter t.
     * 
     * Calculates the point using `origin + t * direction`.
     * 
     * @param t Distance parameter along the ray direction.
     * 
     * @return 4D point in homogeneous coordinates at parameter t.
     */
    Eigen::Vector4d at(double t) const;
};
