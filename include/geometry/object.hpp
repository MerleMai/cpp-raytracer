#pragma once

#include "geometry/hit.hpp"
#include "geometry/ray.hpp"

#include <Eigen/Core>
#include <vector>

/**
 * @brief Base class for all geometric objects in a scene.
 *
 * Primitive objects, transformations, and logical object combinations
 * derive from Object so that heterogeneous object trees can be handled
 * through a common interface.
 */
class Object {
public:
    /// @brief Destroys an object through the polymorphic base interface.
    virtual ~Object() = default;

    /**
     * @brief Computes all intersection points between a ray and this object.
     * 
     * @param ray The ray to test against the geometric object.
     * 
     * @return Collection of Hit objects representing intersections along the ray.
     */
    virtual std::vector<Hit> intersect(const Ray& ray) const = 0;

    /**
     * @brief Tests whether a given point lies inside or on the surface of this object.
     * 
     * @param x 4D point in homogeneous coordinates.
     * 
     * @return True if the point is contained within the object, false otherwise.
     */
    virtual bool contains(const Eigen::Vector4d& x) const = 0;
};
