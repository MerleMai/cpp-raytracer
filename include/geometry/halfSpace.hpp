#pragma once

#include <Eigen/Core>

#include "geometry/material.hpp"
#include "geometry/primitiveObject.hpp"

/**
 * @brief Represents a half-space primitive object.
 *
 * A half-space is defined by a point on its boundary plane and an
 * outward-facing normal vector, together with optical material
 * properties and a refractive index.
 *
 * SceneParser normalizes non-zero input normals before constructing a
 * HalfSpace. Code constructing a HalfSpace directly is responsible for
 * supplying a unit normal when later calculations require one.
 */
class HalfSpace : public PrimitiveObject {
private:
    /// @brief Point on the boundary plane in homogeneous coordinates.
    Eigen::Vector4d position_;

    /// @brief Outward normal vector in homogeneous coordinates.
    Eigen::Vector4d normal_;

public:
    /**
     * @brief Constructs a half-space from its geometric and optical parameters.
     *
     * @param position Point on the boundary plane in homogeneous coordinates.
     * @param normal Outward normal vector in homogeneous coordinates.
     * @param material Optical material coefficients of the half-space.
     * @param index Refractive index of the half-space.
     */
    HalfSpace(
        const Eigen::Vector4d& position,
        const Eigen::Vector4d& normal,
        const Material& material,
        double index
    );

    /**
     * @brief Provides access to the position point on the boundary plane.
     * 
     * @return Constant reference to the position vector in homogeneous coordinates.
     */
    const Eigen::Vector4d& getPosition() const;

   /**
     * @brief Provides access to the outward-facing normal vector of the boundary plane.
     * 
     * @return Constant reference to the unit normal vector in homogeneous coordinates.
     */
    const Eigen::Vector4d& getNormal() const;

    /**
     * @brief Checks if a point lies within the interior or on the boundary of the half-space.
     * 
     * @param x 4D point in homogeneous coordinates.
     * 
     * @return True if the point lies inside or on the boundary plane, false if outside.
     */
    bool contains(const Eigen::Vector4d& x) const override;

    /**
     * @brief Computes the intersection point of a ray with the boundary plane of the half-space.
     * 
     * @param ray The ray tested for intersection.
     * 
     * @return Vector containing the Hit instance if the ray intersects the plane, or empty vector if parallel/behind.
     */
    std::vector<Hit> intersect(const Ray& ray) const override;
};