#pragma once

#include <memory>
#include <vector>

#include <Eigen/Core>

#include "geometry/objectCombination.hpp"

/**
 * @brief Represents the ordered Constructive Solid Geometry (CSG) difference of multiple scene objects.
 *
 * Evaluates the Boolean difference where subsequent objects are subtracted from the primary object:
 * "The object O1 excluding object O2 and all subsequent objects up to ON."
 */
class Difference : public ObjectCombination {
public:
    /**
     * @brief Constructs an ordered object difference.
     *
     * @param children Objects participating in the difference, in order.
     */
    explicit Difference(
        std::vector<std::unique_ptr<Object>> children
    );

    /**
     * @brief Checks if a given point lies inside the CSG difference volume.
     * 
     * @param x 4D point in homogeneous coordinates.
     * 
     * @return True if the point lies inside the base object and outside all subtracted objects.
     */
    bool contains(const Eigen::Vector4d& x) const override;

    /**
     * @brief Computes all valid intersections between a ray and the CSG difference.
     * 
     * Intersections are evaluated by testing ray contacts against all child objects,
     * adjusting normals and boundary state (entering/exiting) for subtracted surfaces,
     * and sorting the resulting hits along the ray.
     * 
     * @param ray The ray to intersect with the difference volume.
     * 
     * @return Sorted vector of valid intersection hits along the ray.
     */
    std::vector<Hit> intersect(const Ray& ray) const override;
};
