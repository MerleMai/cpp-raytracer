#pragma once

#include <Eigen/Core>

#include "geometry/material.hpp"
#include "geometry/primitiveObject.hpp"

/**
 * @brief Represents a spherical primitive object.
 *
 * A sphere is defined by its center position, radius, optical material,
 * and refractive index.
 */
class Sphere : public PrimitiveObject {
private:
    /// @brief Center of the sphere in homogeneous coordinates.
    Eigen::Vector4d position_;

    /// @brief Radius of the sphere.
    double radius_;

public:
    /**
     * @brief Constructs a sphere from its geometric and optical parameters.
     *
     * @param position Center of the sphere in homogeneous coordinates.
     * @param radius Radius of the sphere.
     * @param material Optical material coefficients of the sphere.
     * @param index Refractive index of the sphere.
     */
    Sphere(
        const Eigen::Vector4d& position,
        double radius,
        const Material& material,
        double index
    );

   /**
     * @brief Provides access to the center position of the sphere.
     * 
     * @return Constant reference to the center point in homogeneous coordinates.
     */
    const Eigen::Vector4d& getPosition() const;

    /**
     * @brief Returns the radius of the sphere.
     * 
     * @return Radius value.
     */
    double getRadius() const;

    /**
     * @brief Tests if a point lies inside or on the boundary of the sphere.
     * 
     * @param point 4D point in homogeneous coordinates.
     * 
     * @return True if the distance from the point to the sphere center is less than or equal to the radius.
     */
    bool contains(const Eigen::Vector4d& point) const override;

    /**
     * @brief Calculates ray-sphere intersection points using a quadratic equation.
     * 
     * @param ray Ray to test against the sphere surface.
     * 
     * @return Vector containing up to two Hit structures sorted along the ray direction.
     */
    std::vector<Hit> intersect(const Ray& ray) const override;
};
